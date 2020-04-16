#include <sys/types.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "byte.h"
#include "socket_if.h"
#include "ip.h"

/**
  @file socket_setup.c
  @author djb, feh
  @source ucspi-tcp6
  @brief setup listening socket
*/


int socket_accept(int s,char ip[16],uint16 *port,uint32 *scope_id)
{
  struct sockaddr_in6 sa;
  unsigned int dummy = sizeof(sa);
  int fd;

  fd = accept(s,(struct sockaddr *)&sa,&dummy);
  if (fd == -1) return -1;

  if (sa.sin6_family == AF_INET) {
    struct sockaddr_in *sa4 = (struct sockaddr_in*)&sa;
    byte_copy(ip,12,V4mappedprefix);
    byte_copy(ip+12,4,(char *)&sa4->sin_addr);
    uint16_unpack_big((char *)&sa4->sin_port,port);
    if (scope_id) *scope_id = 0;
    return fd;
  }
  byte_copy(ip,16,(char *)&sa.sin6_addr);
  uint16_unpack_big((char *)&sa.sin6_port,port);
  if (scope_id) *scope_id = sa.sin6_scope_id;

  return fd;
}

int socket_listen(int s,int backlog)
{
  return listen(s,backlog);
}

int socket_ipoptionskill(int s)
{
  if (ipv4socket) 
    return setsockopt(s,IPPROTO_IP,1,(char *) 0,0); /* 1 == IP_OPTIONS */

  return setsockopt(s,IPPROTO_IPV6,1,(char *) 0,0); 
}

int socket_ip6anycast(int s)
{
  int opt = 1;
  int r;

#ifdef GEN_IP_PKTINFO /* Linux */
   r = setsockopt(s,IPPROTO_IP,GEN_IP_PKTINFO,&opt,sizeof(opt));
#elif IP_PKTINFO  /* Solaris */
   r = setsockopt(s,IPPROTO_IP,IP_PKTINFO,&opt,sizeof(opt));
#elif IP_RECVDSTADDR  /* BSD */
   r =  setsockopt(s,IPPROTO_IP,IP_RECVDSTADDR,&opt,sizeof(opt));
#elif IPV6_RECVDSTADDR
   if (!ipv4socket) setsockopt(s,IPPROTO_IPV6,IPV6_ONLY,&opt,sizeof(opt));
   r = setsockopt(s,IPPROTO_IPV6,IP_RECVDSTADDR,&opt,sizeof(opt));
#endif
   return r;
} 

int socket_dualstack(int s)
{
  int opt = 0;

  return setsockopt(s,IPPROTO_IPV6,IPV6_V6ONLY,&opt,sizeof(opt));
}

int socket_nodualstack(int s)
{
  int opt = 1;

  return setsockopt(s,IPPROTO_IPV6,IPV6_V6ONLY,&opt,sizeof(opt));
}


