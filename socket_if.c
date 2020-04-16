#include <sys/types.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include "socket_if.h"

/**
	@file socket_if.c
	@author fefe, feh
	@source ucspi-tcp6
	@brief interface handling for LLU
*/

const unsigned char V4loopback[4] = {127,0,0,1};
const unsigned char V4localnet[4] = {0,0,0,0};
/* the 'V4mappedprefix' constant is needed by 'ip.a' too */
const unsigned char V4mappedprefix[12] = {0,0,0,0, 0,0,0,0, 0,0,0xff,0xff};
const unsigned char V6localnet[16] = {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};
const unsigned char V6loopback[16] = {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1};

uint32 socket_getifidx(const char *ifname) 
{
  return if_nametoindex(ifname);
}

static char ifname[IFNAMSIZ];

const char* socket_getifname(uint32 scope_id) 
{
  char *tmp = if_indextoname(scope_id,ifname);
  if (tmp)
    return tmp;
  else
    return "[unknown]";
}
