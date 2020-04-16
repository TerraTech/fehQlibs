#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include "socket_if.h"
#include "alloc.h"
#include "error.h"
#include "byte.h"
#include "uint_t.h"
#include "ip.h"
#include "dnsresolv.h"

/**
	@file dns_transmit.c
	@authors djb, fefe, feh
	@source qlibs
	@brief DNS query function 
	@comment scope_ids[32] -> 32 LLU root servers supported 
*/

#define DNSPORT 53

int ipv4socket;
uint32 scope_ids[QUERY_MAXNS];

static const int timeouts[5] = { 1, 2, 4, 8, 16 }; /* quadratic, not exponentially */
 
int getscopeid(const struct dns_transmit *d,const char *ip)
{
  int i;

  if (byte_diff(ip,2,V6linklocal)) return 0;
  for (i = 0; i < QUERY_MAXNS; ++i) 
    if (byte_equal(d->servers + 16 * i,16,ip))  
      return scope_ids[i];

  return 0;
}

int serverwantstcp(const char *buf,unsigned int len)
{
  char out[12];

  if (!dns_packet_copy(buf,len,0,out,12)) return 1;
  if (out[2] & 2) return 1;

  return 0;
}

int serverfailed(const char *buf,unsigned int len)
{
  char out[12];
  unsigned int rcode;

  if (!dns_packet_copy(buf,len,0,out,12)) return 1;
  rcode = out[3];
  rcode &= 15;
  if (rcode && (rcode != 3)) { errno = EAGAIN; return 1; }

  return 0;
}

int irrelevant(const struct dns_transmit *d,const char *buf,unsigned int len)
{
  char out[12];
  char *dn;
  unsigned int pos;

  pos = dns_packet_copy(buf,len,0,out,12); if (!pos) return 1;
  if (byte_diff(out,2,d->query + 2)) return 1;
  if (out[4] != 0) return 1;
  if (out[5] != 1) return 1;

  dn = 0;
  pos = dns_packet_getname(buf,len,pos,&dn); if (!pos) return 1;
  if (!dns_domain_equal(dn,d->query + 14)) { alloc_free(dn); return 1; }
  alloc_free(dn);

  pos = dns_packet_copy(buf,len,pos,out,4); if (!pos) return 1;
  if (byte_diff(out,2,d->qtype)) return 1;
  if (byte_diff(out + 2,2,DNS_C_IN)) return 1;

  return 0;
}

void packetfree(struct dns_transmit *d)
{
  if (!d->packet) return;
  alloc_free(d->packet);
  d->packet = 0;
}

void queryfree(struct dns_transmit *d)
{
  if (!d->query) return;
  alloc_free(d->query);
  d->query = 0;
}

void socketfree(struct dns_transmit *d)
{
  if (!d->s1) return;
  close(d->s1 - 1);
  d->s1 = 0;
}

void dns_transmit_free(struct dns_transmit *d)
{
  queryfree(d);
  socketfree(d);
  packetfree(d);
}

int randombind(struct dns_transmit *d)
{
  int j;
 
  for (j = 0; j < 10; ++j) {
    if (socket_bind(d->s1 - 1,d->localip,1025 + dns_random(64510),d->scope_id) == 0)
      return 0;
  } 

  if (socket_bind(d->s1 - 1,d->localip,0,d->scope_id) == 0)
    return 0;

  return DNS_COM;
}

int thisudp(struct dns_transmit *d)
{
  const char *ip;

  socketfree(d);

  while (d->udploop < 5) {
    for (; d->curserver < QUERY_MAXNS; ++d->curserver) {
      ip = d->servers + 16 * d->curserver;
      if (byte_diff(ip,16,V6localnet)) {
        ipv4socket = ip6_isv4mapped(ip);
        d->query[2] = dns_random(256);
        d->query[3] = dns_random(256);
  
        d->s1 = 1 + socket_udp();
        if (!d->s1) { dns_transmit_free(d); return DNS_COM; }
        if (randombind(d) < 0) { dns_transmit_free(d); return DNS_COM; }

        if (byte_equal(ip,2,V6linklocal) && !d->scope_id) 
          d->scope_id = getscopeid(d,ip);
        if (socket_connect(d->s1 - 1,ip,DNSPORT,d->scope_id) == 0)
          if (send(d->s1 - 1,d->query + 2,d->querylen - 2,0) == d->querylen - 2) {
            struct taia now;
            taia_now(&now);
            taia_uint(&d->deadline,timeouts[d->udploop]);
            taia_add(&d->deadline,&d->deadline,&now);
            d->tcpstate = 0;
            return 0;
          }
        socketfree(d);
      }
    }
    ++d->udploop;
    d->curserver = 0;
  }

  dns_transmit_free(d); return DNS_COM;
}

int firstudp(struct dns_transmit *d)
{
  d->curserver = 0;
  return thisudp(d);
}

int nextudp(struct dns_transmit *d)
{
  ++d->curserver;
  return thisudp(d);
}

int thistcp(struct dns_transmit *d)
{
  struct taia now;
  const char *ip;

  socketfree(d);
  packetfree(d);

  for (; d->curserver < QUERY_MAXNS; ++d->curserver) {
    ip = d->servers + 16 * d->curserver;
    if (byte_diff(ip,16,V6localnet)) {
      ipv4socket = ip6_isv4mapped(ip);
      d->query[2] = dns_random(256);
      d->query[3] = dns_random(256);

      d->s1 = 1 + socket_tcp();
      if (!d->s1) { dns_transmit_free(d); return DNS_COM; }
      if (randombind(d) < 0) { dns_transmit_free(d); return DNS_COM; }
  
      taia_now(&now);
      taia_uint(&d->deadline,10);
      taia_add(&d->deadline,&d->deadline,&now);

      if (byte_equal(ip,2,V6linklocal) && !d->scope_id) 
        d->scope_id = getscopeid(d,ip);
      if (socket_connect(d->s1 - 1,ip,DNSPORT,d->scope_id) == 0) {
        d->tcpstate = 2;
        return 0;
      }
      if ((errno == EINPROGRESS) || (errno == EWOULDBLOCK)) {
        d->tcpstate = 1;
        return 0;
      }
  
      socketfree(d);
    }
  }

  dns_transmit_free(d); 
  return DNS_COM;
}

int firsttcp(struct dns_transmit *d)
{
  d->curserver = 0;
  return thistcp(d);
}

int nexttcp(struct dns_transmit *d)
{
  ++d->curserver;
  return thistcp(d);
}

int dns_transmit_start(struct dns_transmit *d,const char servers[QUERY_MAXIPLEN], \
                       int flagrecursive,const char *q,const char qtype[2],const char localip[16])
{
  unsigned int len;

  dns_transmit_free(d);
  errno = EIO;

  len = dns_domain_length(q);
  d->querylen = len + 18;
  d->query = alloc(d->querylen);
  if (!d->query) return DNS_COM;
 
  uint16_pack_big(d->query,len + 16);
  byte_copy(d->query + 2,12,flagrecursive ? "\0\0\1\0\0\1\0\0\0\0\0\0" : "\0\0\0\0\0\1\0\0\0\0\0\0gcc-bug-workaround");
  byte_copy(d->query + 14,len,q);
  byte_copy(d->query + 14 + len,2,qtype);
  byte_copy(d->query + 16 + len,2,DNS_C_IN);

  byte_copy(d->qtype,2,(char *) qtype);
  d->servers = servers;
  byte_copy(d->localip,16,(char *) localip);

  d->udploop = flagrecursive ? 1 : 0;

  if (len + 16 > MSGSIZE) return firsttcp(d);
  return firstudp(d);
}

int dns_transmit_start6(struct dns_transmit *d,const char servers[QUERY_MAXIPLEN], \
                       int flagrecursive,const char *q,const char qtype[2], \
                       const char localip[16],const uint32 scopes[QUERY_MAXNS])
{
  byte_copy(scope_ids,128,(char *) scopes);

  return dns_transmit_start(d,servers,flagrecursive,q,qtype,localip);
}

void dns_transmit_io(struct dns_transmit *d,iopause_fd *x,struct taia *deadline)
{
  x->fd = d->s1 - 1;

  switch (d->tcpstate) {
    case 0: case 3: case 4: case 5:
      x->events = IOPAUSE_READ;
      break;
    case 1: case 2:
      x->events = IOPAUSE_WRITE;
      break;
  }

  if (taia_less(&d->deadline,deadline))
    *deadline = d->deadline;
}

int dns_transmit_get(struct dns_transmit *d,const iopause_fd *x,const struct taia *when)
{
  char udpbuf[MSGSIZE+1];
  unsigned char ch;
  int r;
  int fd;

  errno = EIO;
  fd = d->s1 - 1;

  if (!x->revents) {
    if (taia_less((struct taia *)when,&d->deadline)) return 0;
    errno = ETIMEDOUT;
    if (d->tcpstate == 0) return nextudp(d);
    return nexttcp(d);
  }

/*
have attempted to send UDP query to each server udploop times
have sent query to curserver on UDP socket s
*/
  if (d->tcpstate == 0) {
    r = recv(fd,udpbuf,sizeof(udpbuf),0);
    if (r <= 0) {
      if (errno == ECONNREFUSED) if (d->udploop == 2) return 0;
      return nextudp(d);
    }
    if (r + 1 > sizeof(udpbuf)) return 0;

    if (irrelevant(d,udpbuf,r)) return 0;
    if (serverwantstcp(udpbuf,r)) return firsttcp(d);
    if (serverfailed(udpbuf,r)) {
      if (d->udploop == 2) return 0;
      return nextudp(d);
    }
    socketfree(d);

    d->packetlen = r;
    d->packet = alloc(d->packetlen);
    if (!d->packet) { dns_transmit_free(d); return DNS_COM; }
    byte_copy(d->packet,d->packetlen,udpbuf);
    queryfree(d);
    return 1;
  }

/*
have sent connection attempt to curserver on TCP socket s
pos not defined
*/
  if (d->tcpstate == 1) {
    if (!socket_connected(fd)) return nexttcp(d);
    d->pos = 0;
    d->tcpstate = 2;
    return 0;
  }

/*
have connection to curserver on TCP socket s
have sent pos bytes of query
*/
  if (d->tcpstate == 2) {
    r = write(fd,d->query + d->pos,d->querylen - d->pos);
    if (r <= 0) return nexttcp(d);
    d->pos += r;
    if (d->pos == d->querylen) {
      struct taia now;
      taia_now(&now);
      taia_uint(&d->deadline,10);
      taia_add(&d->deadline,&d->deadline,&now);
      d->tcpstate = 3;
    }
    return 0;
  }

/*
have sent entire query to curserver on TCP socket s
pos not defined
*/
  if (d->tcpstate == 3) {
    r = read(fd,&ch,1);
    if (r <= 0) return nexttcp(d);
    d->packetlen = ch;
    d->tcpstate = 4;
    return 0;
  }

/*
have sent entire query to curserver on TCP socket s
pos not defined
have received one byte of packet length into packetlen
*/
  if (d->tcpstate == 4) {
    r = read(fd,&ch,1);
    if (r <= 0) return nexttcp(d);
    d->packetlen <<= 8;
    d->packetlen += ch;
    d->tcpstate = 5;
    d->pos = 0;
    d->packet = alloc(d->packetlen);
    if (!d->packet) { dns_transmit_free(d); return DNS_COM; }
    return 0;
  }

/*
have sent entire query to curserver on TCP socket s
have received entire packet length into packetlen
packet is allocated
have received pos bytes of packet
*/
  if (d->tcpstate == 5) {
    r = read(fd,d->packet + d->pos,d->packetlen - d->pos);
    if (r <= 0) return nexttcp(d);
    d->pos += r;
    if (d->pos < d->packetlen) return 0;

    socketfree(d);
    if (irrelevant(d,d->packet,d->packetlen)) return nexttcp(d);
    if (serverwantstcp(d->packet,d->packetlen)) return nexttcp(d);
    if (serverfailed(d->packet,d->packetlen)) return nexttcp(d);

    queryfree(d);
    return 1;
  }

  return 0;
}
