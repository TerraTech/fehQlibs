#include "iopause.h"
#include "taia.h"
#include "byte.h"
#include "ip.h"
#include "dnsresolv.h"

/**
	@file dns_resolve.c
	@author djb, fefe, feh
	@source ucspi-tcp
	@brief high-level DNS resolve function
*/

struct dns_transmit dns_resolve_tx = {0};

int dns_resolve(const char *q,const char qtype[2])
{
  struct taia stamp;
  struct taia deadline;
  char servers[QUERY_MAXIPLEN];
  uint32 scopes[QUERY_MAXNS];
  iopause_fd x[1];
  int r;

  if (dns_resolvconfip(servers,scopes) < 0) return DNS_INT;

  if (dns_transmit_start6(&dns_resolve_tx,servers,1,q,qtype,(const char *)V6localnet,scopes) < 0) return DNS_COM;

  for (;;) {
    taia_now(&stamp);
    taia_uint(&deadline,120);
    taia_add(&deadline,&deadline,&stamp);
    dns_transmit_io(&dns_resolve_tx,x,&deadline);
    r = iopause(x,1,&deadline,&stamp);
    if (r < 0) return DNS_COM;
    r = dns_transmit_get(&dns_resolve_tx,x,&stamp);
    if (r < 0) return DNS_COM;
    if (r == 1) return 0;
  }
}
