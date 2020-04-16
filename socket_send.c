#include <sys/types.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "byte.h"
#include "ip.h"
#include "socket_if.h"

/**
	@file socket_send.c
	@author djb, fefe, feh
	@source ucspi-tcp6
	@brief setup sending socket
*/

int ipv4socket;

int socket_send4(int s,const char *buf,unsigned int len,const char ip[4],uint16 port)
{
  struct sockaddr_in sa;

  byte_zero(&sa,sizeof(sa));

  sa.sin_family = AF_INET;
  uint16_pack_big((char *)&sa.sin_port,port);
  byte_copy((char *)&sa.sin_addr,4,ip);

  return sendto(s,buf,len,0,(struct sockaddr *)&sa,sizeof(sa));
}

int socket_send6(int s,const char *buf,unsigned int len,const char ip[16],uint16 port,uint32 scope_id)
{
  struct sockaddr_in6 sa;

  byte_zero(&sa,sizeof(sa));

  sa.sin6_family = AF_INET6;
  sa.sin6_scope_id = scope_id;
  uint16_pack_big((char *)&sa.sin6_port,port);
  byte_copy((char *)&sa.sin6_addr,16,ip);

  return sendto(s,buf,len,0,(struct sockaddr *)&sa,sizeof(sa));
}

int socket_send(int s,const char *buf,unsigned int len,const char ip[16],uint16 port,uint32 scope_id)
{ 
  if (ip6_isv4mapped(ip))
    return socket_send4(s,buf,len,ip+12,port);
  else 
    return socket_send6(s,buf,len,ip,port,scope_id);
} 
