#include <sys/types.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "byte.h"
#include "socket_if.h"
#include "ip.h"

/**
	@file socket_bind.c
	@author djb, fefe, feh
	@source qmail, djbdns, ucspi-tcp6
	@brief binding a socket to a local resource
*/

int ipv4socket;

int socket_bind4(int s,const char ip[4],uint16 port)
{
  struct sockaddr_in sa;

  byte_zero(&sa,sizeof(sa));
  sa.sin_family = AF_INET;
  uint16_pack_big((char *)&sa.sin_port,port);
  byte_copy((char *)&sa.sin_addr,4,ip);

  return bind(s,(struct sockaddr *)&sa,sizeof(sa));
}

int socket_bind4_reuse(int s,const char ip[4],uint16 port)
{
  int opt = 1;
  setsockopt(s,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
  return socket_bind4(s,ip,port);
}

/* seems not to be used here -- djbdns requires it */
void socket_tryreservein(int s,int size)
{
  while (size >= 1024) {
    if (setsockopt(s,SOL_SOCKET,SO_RCVBUF,&size,sizeof(size)) == 0) return;
    size -= (size >> 5);
  }
}

int socket_bind6(int s,const char ip[16],uint16 port,uint32 scope_id)
{
  struct sockaddr_in6 sa;

  byte_zero(&sa,sizeof(sa));
  sa.sin6_family = AF_INET6;
  uint16_pack_big((char *)&sa.sin6_port,port);
/*  implicit: sa.sin6_flowinfo = 0; */
  byte_copy((char *)&sa.sin6_addr,16,ip);
  sa.sin6_scope_id = scope_id;

  return bind(s,(struct sockaddr *)&sa,sizeof(sa));
}

int socket_bind6_reuse(int s,const char ip[16],uint16 port,uint32 scope_id)
{
  int opt = 1;
  setsockopt(s,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
  return socket_bind6(s,ip,port,scope_id);
}

int socket_bind(int s,const char ip[16],uint16 port,uint32 scope_id)
{
  if (ipv4socket)
    return socket_bind4(s,ip + 12,port);
  
  return socket_bind6(s,ip,port,scope_id);
}

int socket_bind_reuse(int s,const char ip[16],uint16 port,uint32 scope_id)
{
  if (ipv4socket)
    return socket_bind4_reuse(s,ip + 12,port);

  return socket_bind6_reuse(s,ip,port,scope_id);
}
