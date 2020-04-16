#include "stralloc.h"
#include "uint_t.h"
#include "byte.h"
#include "ip.h"
#include "dnsresolv.h"

/**
	@file dns_ip.c
	@author djb, fefe, feh
	@source ucspi-tcp6
	@brief DNS IP query
*/

static char *q = 0;

int dns_ip4_packet(stralloc *out,const char *buf,unsigned int len)
{
  unsigned int pos;
  char header[12];
  uint16 numanswers;
  uint16 datalen;

  if (!stralloc_copys(out,"")) return -1;

  pos = dns_packet_copy(buf,len,0,header,12); if (!pos) return -1;
  uint16_unpack_big(header + 6,&numanswers);
  pos = dns_packet_skipname(buf,len,pos); if (!pos) return -1;
  pos += 4;

  while (numanswers--) {
    pos = dns_packet_skipname(buf,len,pos); if (!pos) return -1;
    pos = dns_packet_copy(buf,len,pos,header,10); if (!pos) return -1;
    uint16_unpack_big(header + 8,&datalen);
    if (byte_equal(header,2,DNS_T_A))
      if (byte_equal(header + 2,2,DNS_C_IN))
        if (datalen == 4) {
	        if (!dns_packet_copy(buf,len,pos,header,4)) return -1;
	        if (!stralloc_catb(out,header,4)) return -1;
	      }
    pos += datalen;
  }

  dns_sortip4(out->s,out->len);
  return 0;
}

int dns_ip4(stralloc *out,stralloc *fqdn)
{
  unsigned int i;
  char code;
  char ch;

  if (!stralloc_copys(out,"")) return -1;
  code = 0;

  for (i = 0; i <= fqdn->len; ++i) {
    if (i < fqdn->len)
      ch = fqdn->s[i];
    else
      ch = '.';

    if ((ch == '[') || (ch == ']')) continue;
    if (ch == '.') {
      if (!stralloc_append(out,&code)) return -1;
      code = 0;
      continue;
    }
    if ((ch >= '0') && (ch <= '9')) {
      code *= 10;
      code += ch - '0';
      continue;
    }

    if (!dns_domain_fromdot(&q,fqdn->s,fqdn->len)) return -1;	// fdqn -> A query -> response
    if (dns_resolve(q,DNS_T_A) == -1) return -1;
    if (dns_ip4_packet(out,dns_resolve_tx.packet,dns_resolve_tx.packetlen) == -1) return -1;
    dns_transmit_free(&dns_resolve_tx);
    dns_domain_free(&q);
    return 0;
  }

  out->len &= ~3;
  return 0;
}

int dns_ip6_packet(stralloc *out,const char *buf,unsigned int len)
{
  unsigned int pos;
  char header[16];
  uint16 numanswers;
  uint16 datalen;

  if (!stralloc_cats(out,"")) return -1;

  pos = dns_packet_copy(buf,len,0,header,12); if (!pos) return -1;
  uint16_unpack_big(header + 6,&numanswers);
  pos = dns_packet_skipname(buf,len,pos); if (!pos) return -1;
  pos += 4;

  while (numanswers--) {
    pos = dns_packet_skipname(buf,len,pos); if (!pos) return -1;
    pos = dns_packet_copy(buf,len,pos,header,10); if (!pos) return -1;
    uint16_unpack_big(header + 8,&datalen);
    if (byte_equal(header,2,DNS_T_AAAA)) {
      if (byte_equal(header + 2,2,DNS_C_IN))
        if (datalen == 16) {
          if (!dns_packet_copy(buf,len,pos,header,16)) return -1;
          if (!stralloc_catb(out,header,16)) return -1;
        }
    } else if (byte_equal(header,2,DNS_T_A))
      if (byte_equal(header + 2,2,DNS_C_IN))
        if (datalen == 4) {
          byte_copy(header,12,V4mappedprefix);
          if (!dns_packet_copy(buf,len,pos,header+12,4)) return -1;
          if (!stralloc_catb(out,header,16)) return -1;
        }
    pos += datalen;
  }

  dns_sortip6(out->s,out->len);
  return 0;
}

int dns_ip6(stralloc *out,stralloc *fqdn)
{
  unsigned int i;
  char code;
  char ch;
  char ip[16];

  if (!stralloc_copys(out,"")) return -1;
  if (!stralloc_readyplus(fqdn,1)) return -1;

  fqdn->s[fqdn->len] = 0;		/* if FQDN is just IPv6 */
  if ((i = ip6_scan(fqdn->s,ip))) {
    if (fqdn->s[i]) return -1;
    if (!stralloc_copyb(out,ip,16)) return -1;
    return 0;
  }

  code = 0;
  for (i = 0; i <= fqdn->len; ++i) {
    if (i < fqdn->len)
      ch = fqdn->s[i];
    else
      ch = '.';

    if ((ch == '[') || (ch == ']')) continue;
    if (ch == '.') {
      if (!stralloc_append(out,&code)) return -1;
      code = 0;
      continue;
    }
    if ((ch >= '0') && (ch <= '9')) {
      code *= 10;
      code += ch - '0';
      continue;
    }

    if (!stralloc_copys(out,"")) return -1;

    if (!dns_domain_fromdot(&q,fqdn->s,fqdn->len)) return -1;	// fqdn -> AAAA query -> response
    if (dns_resolve(q,DNS_T_AAAA) != -1)
      if (dns_ip6_packet(out,dns_resolve_tx.packet,dns_resolve_tx.packetlen) != -1) {
        dns_transmit_free(&dns_resolve_tx);
        dns_domain_free(&q);
      }

    if (!dns_domain_fromdot(&q,fqdn->s,fqdn->len)) return -1;	// fqdn -> A query -> response
    if (dns_resolve(q,DNS_T_A) != -1)
      if (dns_ip6_packet(out,dns_resolve_tx.packet,dns_resolve_tx.packetlen) != -1) {
        dns_transmit_free(&dns_resolve_tx);
        dns_domain_free(&q);
      }

    return out->a>0?0:-1;
  }

  out->len &= ~3;
  return 0;
}
