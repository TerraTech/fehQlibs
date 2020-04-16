#include <sys/types.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include "byte.h"
#include "socket_if.h"
#include "ip.h"

/**
	@file socket_connect.c
	@author djb, fefe, feh, kp
	@source qmail, ucscpi-tcp6
	@brief connection to remote IP host
*/

int ipv4socket;

int socket_connect6(int s,const char ip[16],uint16 port,uint32 scope_id)
{
  struct sockaddr_in6 sa;

  byte_zero(&sa,sizeof(sa));
  sa.sin6_family = AF_INET6;
  uint16_pack_big((char *)&sa.sin6_port,port);
  sa.sin6_flowinfo = 0;
  sa.sin6_scope_id = scope_id;
  byte_copy((char *)&sa.sin6_addr,16,ip);

  return connect(s,(struct sockaddr *)&sa,sizeof(sa));
}

/* this explizit declaration is needed to prevent compiler warnings */
int read(int _str, void *_buf, int _b);

int socket_connect4(int s,const char ip[4],uint16 port)
{
  struct sockaddr_in sa;

  byte_zero(&sa,sizeof(sa));
  sa.sin_family = AF_INET;
  uint16_pack_big((char *)&sa.sin_port,port);
  byte_copy((char *)&sa.sin_addr,4,ip);

  return connect(s,(struct sockaddr *)&sa,sizeof(sa));
}

int socket_connect(int s,const char ip[16],uint16 port,uint32 scope_id)
{
  if (ipv4socket || ip6_isv4mapped(ip))
    return socket_connect4(s,ip+12,port);

  return socket_connect6(s,ip,port,scope_id);
}

int socket_connected(int s)
{
  struct sockaddr_in6 sa;
  int dummy;
  char ch;

  dummy = sizeof(sa);
  if (getpeername(s,(struct sockaddr *)&sa,(socklen_t *)&dummy) == -1) {
    read(s,&ch,1); /* sets errno */
    return 0;
  }
  return 1;
}
