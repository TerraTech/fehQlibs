#include <sys/types.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "byte.h"
#include "socket_if.h"
#include "ip.h"

/**
	@file socket_info.c
	@author djb, fefe, feh
	@source ucspi-tcp6
	@brief querying local and remote info for socket
*/

int socket_local(int s,char ip[16],uint16 *port,uint32 *scope_id)
{
  struct sockaddr_in6 sa;
  unsigned int dummy = sizeof(sa);

  if (getsockname(s,(struct sockaddr *)&sa,&dummy) == -1) return -1;
  
  if (sa.sin6_family == AF_INET) {
    struct sockaddr_in *sa4 = (struct sockaddr_in*)&sa;
    byte_copy(ip,12,V4mappedprefix);
    byte_copy(ip+12,4,(char *)&sa4->sin_addr);
    uint16_unpack_big((char *)&sa4->sin_port,port);
    if (scope_id) *scope_id = 0;
    return 0;
  }
  byte_copy(ip,16,(char *)&sa.sin6_addr);
  uint16_unpack_big((char *)&sa.sin6_port,port);
  if (scope_id) *scope_id = sa.sin6_scope_id;

  return 0;
}

int socket_remote(int s,char ip[16],uint16 *port,uint32 *scope_id)
{
  struct sockaddr_in6 sa;
  unsigned int dummy = sizeof(sa);

  if (getpeername(s,(struct sockaddr *)&sa,&dummy) == -1) return -1;

  if (sa.sin6_family == AF_INET) {
    struct sockaddr_in *sa4 = (struct sockaddr_in*)&sa;
    byte_copy(ip,12,V4mappedprefix);
    byte_copy(ip+12,4,(char *)&sa4->sin_addr);
    uint16_unpack_big((char *)&sa4->sin_port,port);
    *scope_id = 0;
    return 0;
  }

  byte_copy(ip,16,(char *)&sa.sin6_addr);
  uint16_unpack_big((char *)&sa.sin6_port,port);
  *scope_id = sa.sin6_scope_id;

  return 0;
}
