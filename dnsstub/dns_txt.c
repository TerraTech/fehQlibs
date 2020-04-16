#include "stralloc.h"
#include "uint_t.h"
#include "byte.h"
#include "dnsresolv.h"

/**
	@file dns_txt.c
	@author djb
	@source ucspi-tcp
	@brief DNS TXT query
*/

int dns_txt_packet(stralloc *out,const char *buf,unsigned int len)
{
  unsigned int pos;
  char header[12];
  uint16 numanswers;
  uint16 datalen;
  char ch;
  unsigned int txtlen;
  int i;
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
    if (byte_equal(header,2,DNS_T_TXT))
      if (byte_equal(header + 2,2,DNS_C_IN)) {
        if (pos + datalen > len) return DNS_ERR;
        txtlen = 0;
        for (i = 0; i < datalen; ++i) {
          ch = buf[pos + i];
          if (!txtlen)
            txtlen = (unsigned char) ch;
          else {
            --txtlen;
            if (ch < 32) ch = '?';
            if (ch > 126) ch = '?';
            if (!stralloc_append(out,&ch)) return DNS_MEM;
          }
        }
      }
    pos += datalen;
    ++ranswers;
    if (numanswers) if (!stralloc_append(out,"\n")) return DNS_MEM;
  }

  return ranswers;
}

static char *q = 0;

int dns_txt(stralloc *out,const stralloc *fqdn)
{
  int rc;

  if (!dns_domain_fromdot(&q,fqdn->s,fqdn->len)) return DNS_ERR;
  if (dns_resolve(q,DNS_T_TXT) < 0) return DNS_ERR;
  if ((rc = dns_txt_packet(out,dns_resolve_tx.packet,dns_resolve_tx.packetlen)) < 0) return DNS_ERR;
  dns_transmit_free(&dns_resolve_tx);
  dns_domain_free(&q);

  return rc; 
}
