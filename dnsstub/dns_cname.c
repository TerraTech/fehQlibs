#include "stralloc.h"
#include "uint_t.h"
#include "byte.h"
#include "ip.h"
#include "dnsresolv.h"

/**
	@file dns_cname.c
	@author feh
	@brief dns cname lookup
*/

static char *q = 0;

int dns_cname_packet(stralloc *out,const char *buf,unsigned int len)
{
  unsigned int pos;
  char header[12];
  uint16 numanswers;
  uint16 datalen;
  int ranswers = 0;

  if (!stralloc_copys(out,"")) return DNS_MEM;

  pos = dns_packet_copy(buf,len,0,header,12); if (!pos) return DNS_ERR;
  uint16_unpack_big(header + 6,&numanswers);
  pos = dns_packet_skipname(buf,len,pos); if (!pos) return DNS_ERR;
  pos += 4;

  while (numanswers--) {
    pos = dns_packet_skipname(buf,len,pos); if (!pos) return DNS_ERR;
    pos = dns_packet_copy(buf,len,pos,header,10); if (!pos) return DNS_ERR;
    uint16_unpack_big(header + 8,&datalen);
    if (byte_equal(header,2,DNS_T_CNAME))
      if (byte_equal(header + 2,2,DNS_C_IN)) {
        if (!dns_packet_getname(buf,len,pos,&q)) return DNS_ERR;
        if (!dns_domain_todot_cat(out,q)) return DNS_ERR;
      }
    pos += datalen;
    ++ranswers;
  }

  return ranswers;
}

int dns_cname(stralloc *out,stralloc *fqdn)
{
  int rc; 

  if (!dns_domain_fromdot(&q,fqdn->s,fqdn->len)) return DNS_ERR;
  if (dns_resolve(q,DNS_T_CNAME) < 0) return DNS_ERR;
  if ((rc = dns_cname_packet(out,dns_resolve_tx.packet,dns_resolve_tx.packetlen)) < 0) return DNS_ERR;
  dns_transmit_free(&dns_resolve_tx);
  dns_domain_free(&q);

  return rc;
}
