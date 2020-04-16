#include "stralloc.h"
#include "uint_t.h"
#include "byte.h"
#include "ip.h"
#include "dnsresolv.h"

/**
	@file dns_name.c
	@author djb, fefe, feh
	@source ucspi-tcp
	@brief dns name query (ptr)
*/

static char *q = 0;

int dns_name_packet(stralloc *out,const char *buf,unsigned int len)
{
  unsigned int pos;
  char header[12];
  uint16 numanswers;
  uint16 datalen;

  if (!stralloc_copys(out,"")) return DNS_MEM;

  pos = dns_packet_copy(buf,len,0,header,12); if (!pos) return DNS_ERR;
  uint16_unpack_big(header + 6,&numanswers);
  pos = dns_packet_skipname(buf,len,pos); if (!pos) return DNS_ERR;
  pos += 4;

  while (numanswers--) {
    pos = dns_packet_skipname(buf,len,pos); if (!pos) return DNS_ERR;
    pos = dns_packet_copy(buf,len,pos,header,10); if (!pos) return DNS_ERR;
    uint16_unpack_big(header + 8,&datalen);
    if (byte_equal(header,2,DNS_T_PTR))
      if (byte_equal(header + 2,2,DNS_C_IN)) {
        if (!dns_packet_getname(buf,len,pos,&q)) return DNS_ERR;
        if (!dns_domain_todot_cat(out,q)) return DNS_ERR;
        return 1;
      }
    pos += datalen;
  }

  return 0;
}

int dns_name4(stralloc *out,const char ip[4])
{
  int rc;
  char name[DNS_NAME4_DOMAIN];

  dns_name4_domain(name,ip);
  if (dns_resolve(name,DNS_T_PTR) < 0) return DNS_ERR;
  if ((rc = dns_name_packet(out,dns_resolve_tx.packet,dns_resolve_tx.packetlen)) < 0) return DNS_ERR;
  dns_transmit_free(&dns_resolve_tx);
  dns_domain_free(&q);

  return rc;
}

int dns_name6(stralloc *out,const char ip[16])
{
  int rc;
  char name[DNS_NAME6_DOMAIN];

  dns_name6_domain(name,ip);
  if (dns_resolve(name,DNS_T_PTR) < 0) return DNS_ERR;
  if ((rc = dns_name_packet(out,dns_resolve_tx.packet,dns_resolve_tx.packetlen)) < 0) return DNS_ERR;
  dns_transmit_free(&dns_resolve_tx);
  dns_domain_free(&q);

  return rc;
}

int dns_name(stralloc *out,const char ip[16])
{
  if (ip6_isv4mapped(ip))
    return dns_name4(out,ip+12);
  else 
    return dns_name6(out,ip);
}
