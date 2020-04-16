#include "taia.h"
#include "readclose.h"
#include "byte.h"
#include "ip.h"
#include "env.h"
#include "dnsresolv.h"
#include "socket_if.h"

/**
	@file dns_rcip.c
	@author djb, fefe, feh
	@source ucspi-tcp
	@brief dns receive for query
*/

static stralloc data = {0};
static stralloc ifname = {0};

static int init(char ip[QUERY_MAXIPLEN],uint32 sid[QUERY_MAXNS])
{
  int i;
  int j;
  int k = 0;
  int iplen = 0;
  char *x;
  char ip4[4];

/* Read (compactified) IPv4|v6 addresses of resolvers
   Store them in array IP with fixed length : 
   ip(64) -> 16 IPv4 addresses (not used anymore)
   ip(512) -> 16*2 IPv6 addresses (we use IPv4 mapped IPv6 addresses) 
   sid(32) -> the scope for the respective IPv6 or 0
*/
  for (i = 0; i < QUERY_MAXNS; ++i) sid[i] = 0;

  x = env_get("DNSCACHEIP");
  if (x)
    while (iplen <= 240 && *x != '\0') {
      if (*x == ' ') 
        ++x; 
      else 
        if ((i = ip6_ifscan(x,ip + iplen,&ifname))) {
          if (ifname.len > 2) sid[k] = socket_getifidx(ifname.s);
          iplen += 16; k++;
          if (*(x += i) == '\0') break;
        } 
    }

  if (!iplen) {
    i = openreadclose("/etc/resolv.conf",&data,64);
    if (i == -1) return DNS_INT;
    if (i) {
      if (!stralloc_append(&data,"\n")) return DNS_MEM;
      i = 0;
      for (j = 0; j < data.len; ++j)
        if (data.s[j] == '\n') {
          if (byte_equal("nameserver ",11,data.s + i) || byte_equal("nameserver\t",11,data.s + i)) {
            i += 10; 
            while ((data.s[i] == ' ') || (data.s[i] == '\t'))
              i++; 
            if (iplen <= 240) {
              data.s[j] = '\0'; /* ip6_ifscan needs terminated string on input */
              if (ip4_scan(data.s + i,ip4)) {
                if (byte_equal(ip4,4,"\0\0\0\0"))
                  byte_copy(ip4,4,"\177\0\0\1");
                byte_copy(ip+iplen,12,V4mappedprefix);
                byte_copy(ip+iplen+12,4,ip4);
                sid[k] = 0; iplen += 16; k++;
              } else if (ip6_ifscan(data.s + i,ip + iplen,&ifname)) {
                if (ifname.len > 2) sid[k] = socket_getifidx(ifname.s);
                iplen += 16; k++;
              }
            }
          }
          i = j + 1;
        }
    }
  }

  if (!iplen) {
    byte_copy(ip,16,"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\1");
    iplen = 16;
  }
  byte_zero(ip + iplen,QUERY_MAXIPLEN - iplen);
  return 0;
}

static int ok = 0;
static unsigned int uses;
static struct taia deadline;
static char ip[QUERY_MAXIPLEN]; /* defined if ok */
static uint32 scopes[QUERY_MAXNS]; 

int dns_resolvconfip(char s[QUERY_MAXIPLEN],uint32 scope[QUERY_MAXNS])
{
  struct taia now;

  taia_now(&now);
  if (taia_less(&deadline,&now)) ok = 0;
  if (!uses) ok = 0;

  if (!ok) {
    if (init(ip,scopes) < 0) return DNS_INT;
    taia_uint(&deadline,600);
    taia_add(&deadline,&now,&deadline);
    uses = 10000;
    ok = 1;
  }

  --uses;
  byte_copy(s,QUERY_MAXIPLEN,ip);
  byte_copy(scope,128,scopes);
  return 0;
}
