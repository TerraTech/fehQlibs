#include <sys/types.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include "close.h"         /* better use unistd.h ? */
#include "ndelay.h"
#include "socket_if.h"
#include "error.h"

#ifndef EAFNOSUPPORT
#define EAFNOSUPPORT EINVAL
#endif

/**
	@file socket_udp.c
	@author djb, fefe, feh
	@source ucspi-tcp6
	@brief setup a UDP message socket
*/

extern int ipv4socket;

int socket_udp4(void)
{
  int s;

  s = socket(PF_INET,SOCK_DGRAM,0);
  if (s == -1) return -1;
  if (ndelay_on(s) == -1) { close(s); return -1; }

  return s;
}

int socket_udp6(void)
{
  int s;

  s = socket(PF_INET6,SOCK_DGRAM,0);
  if (s == -1) 
    if (errno == EINVAL || errno == EAFNOSUPPORT || errno == EPFNOSUPPORT || errno == EPROTONOSUPPORT)
      return socket_udp4();

  if (ndelay_on(s) == -1) { close(s); return -1; }

  return s;
}

int socket_udp(void)
{
  if (ipv4socket) 
    return socket_udp4();

  return socket_udp6();
}
