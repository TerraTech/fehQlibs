#ifndef DNSRESOLV_H
#define DNSRESOLV_H

/*
 *  Revision 20190730, Erwin Hoffmann
 *  - revised DNS_* return codes to make them compliant with ucspi-*
 *  Revision 20190430, Erwin Hoffmann
 *  - added DNS_SOFT/HARD/MEM complient to s/qmail
 *  - code changes in all decendent modules
 *  Revision 20180222, Erwin Hoffmann
 *  - we consider in total 32 NS IPs (IPv4 + IPv6)
 *  - added dns_transmit_start6
 *  - added uint32 scope_ids[32], 
 *    the initial NS scopes read from /etc/resolv.conf et al.
 *  Revision 20180118, Erwin Hoffmann
 *  - included MSGSIZE for DNS messages (instead of MTUSIZE)
 *  Revision 20171231, Erwin Hoffmann
 *  - renamed to dnsresolv.h and removed *qmail declarations
 *  Revision 20170902, Erwin Hoffmann
 *  - added old definitions from *qmail for (temp) backwards compatibility
 *  - added more DNS RR definitions
*/

#include "stralloc.h"
#include "iopause.h"
#include "taia.h"

/* Note: The conventions are subject of change in forthcoming versions */

#define DNS_MEM  -1
#define DNS_ERR  -2              /* parsing errors and others */
#define DNS_COM  -4              /* (socket) communication errors */
#define DNS_INT  -4              /* internal errors */

#define MSGSIZE MTUSIZE       /* aggressive */
// #define MSGSIZE 512              /* RFC 1035 */

#define QUERY_MAXNS 32           /* 16 IPv4 + 16 IPv6 NS */
#define QUERY_MAXIPLEN 512       /* QUERY_MAXNS * 16 */

/* Note: These following definitions are subject of change */

#define DNS_C_IN "\0\1"
#define DNS_C_ANY "\0\377"

#define DNS_T_A "\0\1"
#define DNS_T_NS "\0\2"
#define DNS_T_CNAME "\0\5"
#define DNS_T_SOA "\0\6"
#define DNS_T_PTR "\0\14"
#define DNS_T_HINFO "\0\15"
#define DNS_T_MX "\0\17"
#define DNS_T_TXT "\0\20"
#define DNS_T_RP "\0\21"
#define DNS_T_SIG "\0\30"
#define DNS_T_KEY "\0\31"
#define DNS_T_AAAA "\0\34"
#define DNS_T_NAPTR "\0\43"
#define DNS_T_CERT "\0\45"
#define DNS_T_OPT "\0\51"
#define DNS_T_DS "\0\53"
#define DNS_T_SSHFP "\0\54"
#define DNS_T_RRSIG "\0\56"
#define DNS_T_DNSKEY "\0\60"
#define DNS_T_NSEC3 "\0\62"
#define DNS_T_TLSA "\0\63"
#define DNS_T_SPF "\0\143"
#define DNS_T_AXFR "\0\374"
#define DNS_T_ANY "\0\377"
#define DNS_T_CAA "\1\1"

struct dns_transmit {
  char *query;  /* 0, or dynamically allocated */
  unsigned int querylen;
  char *packet; /* 0, or dynamically allocated */
  unsigned int packetlen;
  int s1; /* 0, or 1 + an open file descriptor */
  int tcpstate;
  unsigned int udploop;
  unsigned int curserver;
  struct taia deadline;
  unsigned int pos;
  const char *servers;
  uint32 scope_id;
  char localip[16];
  char qtype[2];
} ;

/* General */

extern void dns_random_init(const char *);
extern unsigned int dns_random(unsigned int);

extern void dns_domain_free(char **);
extern int dns_domain_copy(char **,const char *);
extern unsigned int dns_domain_length(const char *);
extern int dns_domain_equal(const char *,const char *);
extern int dns_domain_suffix(const char *,const char *);
extern unsigned int dns_domain_suffixpos(const char *,const char *);
extern int dns_domain_fromdot(char **,const char *,unsigned int);
extern int dns_domain_todot_cat(stralloc *,const char *);

extern unsigned int dns_packet_copy(const char *,unsigned int,unsigned int,char *,unsigned int);
extern unsigned int dns_packet_getname(const char *,unsigned int,unsigned int,char **);
extern unsigned int dns_packet_skipname(const char *,unsigned int,unsigned int);

extern struct dns_transmit dns_resolve_tx;
extern int dns_transmit_start(struct dns_transmit *,const char *,int,const char *,const char *,const char *);
extern void dns_transmit_free(struct dns_transmit *);
extern void dns_transmit_io(struct dns_transmit *,iopause_fd *,struct taia *);
extern int dns_transmit_get(struct dns_transmit *,const iopause_fd *,const struct taia *);

/* Common IPv4 + IPv6 */

extern int dns_resolvconfip(char *,uint32 *);
extern int dns_resolvconfrewrite(stralloc *);
extern int dns_resolve(const char *,const char *);

extern int dns_name(stralloc *,const char *);
extern int dns_name_packet(stralloc *,const char *,unsigned int);
extern int dns_txt_packet(stralloc *,const char *,unsigned int);
extern int dns_txt(stralloc *,const stralloc *);
extern int dns_mx_packet(stralloc *,const char *,unsigned int);
extern int dns_mx(stralloc *,const stralloc *);
extern void dns_sortip(char *,unsigned int); 

/* IPv4 specific */

extern int dns_ip4_packet(stralloc *,const char *,unsigned int);
extern int dns_ip4(stralloc *,stralloc *);
extern void dns_sortip4(char *,unsigned int);

extern int dns_ip4_qualify_rules(stralloc *,stralloc *,const stralloc *,const stralloc *);
extern int dns_ip4_qualify(stralloc *,stralloc *,const stralloc *);

#define DNS_NAME4_DOMAIN 31
extern int dns_name4_domain(char *,const char *);
extern int dns_name4(stralloc *,const char *);

/* IPv6 specific */

extern int dns_ip6_packet(stralloc *,const char *,unsigned int);
extern int dns_ip6(stralloc *,stralloc *);
extern void dns_sortip6(char *,unsigned int);

extern int dns_ip6_qualify_rules(stralloc *,stralloc *,const stralloc *,const stralloc *);
extern int dns_ip6_qualify(stralloc *,stralloc *,const stralloc *);

#define DNS_NAME6_DOMAIN (4*16+11)
extern int dns_name6_domain(char *,const char *);
extern int dns_name6(stralloc *,const char *);

extern int dns_cname_packet(stralloc *,const char *,unsigned int);
extern int dns_cname(stralloc *,stralloc *);

extern int dns_transmit_start6(struct dns_transmit *,const char *,int,const char *,const char *,const char *,const uint32 *);

/* General */

extern void socketfree(struct dns_transmit *);
extern void queryfree(struct dns_transmit *);
extern void packetfree(struct dns_transmit *);
extern int randombind(struct dns_transmit *);
extern int serverwantstcp(const char *,unsigned int);
extern int serverfailed(const char *,unsigned int);
extern int getscopeid(const struct dns_transmit *,const char *);
extern int firstudp(struct dns_transmit *);
extern int nextudp(struct dns_transmit *);
extern int firsttcp(struct dns_transmit *);
extern int nexttcp(struct dns_transmit *);

#endif
