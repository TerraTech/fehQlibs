#include "byte.h"
#include "fmt.h"
#include "ip.h"
#include "dnsresolv.h"

/**
	@file dns_nd.c
	@autor djb, fefe
	@source ucspi-tcp
	@brief dns domain name for ip (wire format)
*/

int dns_name4_domain(char name[DNS_NAME4_DOMAIN],const char ip[4])
{
  unsigned int namelen;
  unsigned int i;

  namelen = 0;
  i = fmt_ulong(name + namelen + 1,(unsigned long) (unsigned char) ip[3]);
  name[namelen++] = i;
  namelen += i;
  i = fmt_ulong(name + namelen + 1,(unsigned long) (unsigned char) ip[2]);
  name[namelen++] = i;
  namelen += i;
  i = fmt_ulong(name + namelen + 1,(unsigned long) (unsigned char) ip[1]);
  name[namelen++] = i;
  namelen += i;
  i = fmt_ulong(name + namelen + 1,(unsigned long) (unsigned char) ip[0]);
  name[namelen++] = i;
  namelen += i;
  byte_copy(name + namelen,14,"\7in-addr\4arpa\0");
  return namelen+14;
}

int dns_name6_domain(char name[DNS_NAME6_DOMAIN],const char ip[16])
{
  unsigned int j;

  for (j = 0; j < 16; j++) {
    name[j*4]   = 1;
    name[j*4+1] = tohex(ip[15-j] & 15);
    name[j*4+2] = 1;
    name[j*4+3] = tohex((unsigned char)ip[15-j] >> 4);
  }
  byte_copy(name + 4*16,10,"\3ip6\4arpa\0");
  return 4*16+10;
}

