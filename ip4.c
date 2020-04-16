#include "fmt.h"
#include "scan.h"
#include "str.h"
#include "ip.h"

/**
	@file ip4.c
	@author djb, fefe, feh, kp
	@source ucspi-tcp, ucspi-tcp6 
	@brief handling of IPv4 addresses
*/

/**
 @brief ip4_fmt
        converts IPv4 address to dotted decimal string format
 @param input:  IPv4 char array
        output: IPv4 address string
 @return int length of address (ok > 0)
 */

unsigned int ip4_fmt(char *s,char ip[4])
{
  unsigned int len;
  unsigned int i;

  len = 0;
  i = fmt_ulong(s,(unsigned long) (unsigned char) ip[0]); len += i; if (s) s += i;
  if (s) { *s++ = '.'; } ++len;
  i = fmt_ulong(s,(unsigned long) (unsigned char) ip[1]); len += i; if (s) s += i;
  if (s) { *s++ = '.'; } ++len;
  i = fmt_ulong(s,(unsigned long) (unsigned char) ip[2]); len += i; if (s) s += i;
  if (s) { *s++ = '.'; } ++len;
  i = fmt_ulong(s,(unsigned long) (unsigned char) ip[3]); len += i; if (s) s += i;
  return len;
}

/**
 @brief ia4_fmt
        converts IPv4 address into DNS inverse nibble format
 @param input:  IPv4 char array
        output: IPv4 address string
 @return int length of address (ok > 0)
 */

unsigned int ia4_fmt(char *s,char ip[4])
{
  unsigned int i;
  unsigned int len;

  len = 0;
  i = fmt_ulong(s,(unsigned long) ip[3]); len += i; if (s) s += i;
  i = fmt_str(s,"."); len += i; if (s) s += i;
  i = fmt_ulong(s,(unsigned long) ip[2]); len += i; if (s) s += i;
  i = fmt_str(s,"."); len += i; if (s) s += i;
  i = fmt_ulong(s,(unsigned long) ip[1]); len += i; if (s) s += i;
  i = fmt_str(s,"."); len += i; if (s) s += i;
  i = fmt_ulong(s,(unsigned long) ip[0]); len += i; if (s) s += i;
  i = fmt_str(s,".in-addr.arpa."); len += i; if (s) s += i;
  return len;
}

/**
 @brief ip4_scan
        parse IPv4 address string and convert to IP address array
 @param input:  IPv4 address string
        output: IPv4 char array
 @return int length of ip_address (ok > 0)
 */

unsigned int ip4_scan(const char *s,char ip[4])
{
  unsigned int i;
  unsigned int len;
  unsigned long u;

  byte_zero(ip,4);
  len = 0;
  i = scan_ulong((char *)s,&u); if (!i) { return 0; } ip[0] = u; s += i; len += i;
  if (*s != '.') { return 0; } ++s; ++len;
  i = scan_ulong((char *)s,&u); if (!i) { return 0; } ip[1] = u; s += i; len += i;
  if (*s != '.') { return 0; } ++s; ++len;
  i = scan_ulong((char *)s,&u); if (!i) { return 0; } ip[2] = u; s += i; len += i;
  if (*s != '.') { return 0; } ++s; ++len;
  i = scan_ulong((char *)s,&u); if (!i) { return 0; } ip[3] = u; s += i; len += i;
  return len;
}

/**
 @brief ip4_scanbracket
        parse IPv4 address string enclosed in brackets and convert to IP address array
 @param input:  IPv4 char array
        output: IPv4 char array
 @return int lenth of ip_address (ok > 0)
 */

unsigned int ip4_scanbracket(const char *s,char ip[4])
{
  unsigned int len;

  if (*s != '[') return 0;
  len = ip4_scan(s + 1,ip);
  if (!len) return 0;
  if (s[len + 1] != ']') return 0;
  return len + 2;
}

/**
 @brief ip4_cidr
        parse IPv4 address string
        concatinated with the prefix length:  192.168.1/24
 @param input:  IPv6 char array
        output: IPv6 char array, long plen
 @return int length of ip6_address/ip
 */

unsigned int ip4_cidr(char *s,char ip[4],unsigned long *plen)
{
  unsigned int j = 0;
  *plen = 32UL;

  j = str_chr(s,'/');
  if (s[j] == '/') {
     s[j] = 0;
     j = scan_ulong(s+j+1,plen);
  } 
  return ip4_scan((const char *)s,ip);
}

/**
 @brief ip4_bytestring 
        parse IPv4 address and represent as char string with length prefix
 @param input:  IPv4 char array, prefix length
        output: pointer to stralloc bytestring
 @return 0, if ok; 1, memory shortage; 2, input error
 */

unsigned int ip4_bytestring(stralloc *ipstring,char ip[4],int prefix)
{
  int i, j;
  unsigned char number;

  ipstring->len = 0;
  if (!stralloc_copys(ipstring,"")) return 1;
  if (!stralloc_readyplus(ipstring,32)) return 1;

  for (i = 0; i < 4; i++) {
    number = (unsigned char) ip[i];
    if (number > 255) return 2;

    for (j = 7; j >= 0; j--) {
      if (number & (1<<j)) {
        if (!stralloc_cats(ipstring,"1")) return 1;
      } else {
        if (!stralloc_cats(ipstring,"0")) return 1;
      }
      if (prefix == 0) {
        if (!stralloc_0(ipstring)) return 1;
        return 0;
      }
      prefix--;
    }
  }
  if (!stralloc_0(ipstring)) return 1;

  return 0;
}
