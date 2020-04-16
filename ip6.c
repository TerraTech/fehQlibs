#include "fmt.h"
#include "byte.h"
#include "scan.h"
#include "ip.h"
#include "str.h"

/**
	@file ip6.c
	@author djb, fefe, feh
	@source ucspi-tcp, ucspi-tcp6
	@brief handling of IPv6 addresses
*/

/***
 @brief ip6_fmt
        convert IPv6 address to compactified IPv6 address string
 @param input:  IPv6 char array
        output: pointer to IPv6 address string
 @return int length of address (ok > 0)
 */

unsigned int ip6_fmt(char *s,char ip[16])
{
  unsigned int len;
  unsigned int i;
  unsigned int temp, temp0;
  unsigned int compressing;
  unsigned int compressed;
  int j;

  len = 0;
  compressing = 0;
  compressed = 0;

  for (j = 0; j < 16; j += 2) {
    if (j == 12 && ip6_isv4mapped(ip)) {
      len += ip4_fmt(s,ip+12);
      break;
    }

    temp = ((unsigned long) (unsigned char) ip[j] << 8) +
            (unsigned long) (unsigned char) ip[j+1];

    temp0 = 0;
    if (!compressing && j < 16)
      temp0 = ((unsigned long) (unsigned char) ip[j+2] << 8) +
               (unsigned long) (unsigned char) ip[j+3];

    if (temp == 0 && temp0 == 0 && !compressed) {
      if (!compressing) {
        compressing = 1;
        if (j == 0) {
          if (s) *s++ = ':';
          ++len;
        }
      }
    } else {
      if (compressing) {
        compressing = 0;
        ++compressed;
        if (s) *s++ = ':';
        ++len;
      }
      i = fmt_xlong(s,temp);
      len += i;
      if (s) s += i;
      if (j < 14) {
        if (s) *s++ = ':';
        ++len;
      }
    }
  }
  if (compressing) { *s++ = ':'; ++len; }

  return len;
}

/***
 @brief ip6_fmt_flat
        convert IPv6 address to IPv6 address string
 @param input:  IPv6 char array
        output: pointer to IPv6 address string
 @return int length of address (ok > 0)
 */

unsigned int ip6_fmt_flat(char *s,char ip[16])
{
  int i;
  for (i = 0; i < 16; i++) {
    *s++ = tohex((unsigned char)ip[i] >> 4);
    *s++ = tohex((unsigned char)ip[i] & 15);
  }
  return 32;
}

/***
 @brief ia6_fmt
        convert IPv6 address to inverse DNS nibble format
        1.2.3.4.5.6.7.8.9.a.b.c.d.e.f.1.2.3.4.5.6.7.8.9.a.b.c.d.e.f.ip6.arpa 
 @param input:  IPv6 char array
        output: pointer to IPv6 address string
 @return int length of address 
 */

unsigned int ia6_fmt(char *s,char ip[16])
{
  unsigned int i;
  unsigned int len;
  int j;

  static char data[] = "0123456789abcdef";
  len = 0;

  for (j = 15; j >= 0; j--) {
    i = fmt_str(s,&data[ip[j] & 0x0f]); len += i; if (s) s += i;
    i = fmt_str(s,"."); len += i; if (s) s += i;
    i = fmt_str(s,&data[ip[j] >> 4 & 0x0f]); len += i; if (s) s += i;
    i = fmt_str(s,"."); len += i; if (s) s += i;
  }
  i = fmt_str(s,"ip6.arpa."); len += i; if (s) s += i;

  return len;
}


/***
 @brief ip6_scan_flat
        convert IPv6 address string to IPv6 address array
 @param input:  pointer to IPv6 address string
        output: IPv6 char array
 @return int length of address (ok > 0)
 */

unsigned int ip6_scan_flat(const char *s,char ip[16])
{
  int i, tmp;

  for (i = 0; i < 16; i++) {
    tmp = fromhex(*s++);
    if (tmp < 0) return 0;
    ip[i] = tmp << 4;
    tmp = fromhex(*s++);
    if (tmp < 0) return 0;
    ip[i] += tmp;
  }
  return 32;
}

/***
 @brief ip6_scan
        parse compactified IPv6 address string and convert to IPv6 address array
 @param input:  pointer to IPv6 address string
        output: IPv6 char array
 @return int length of ip6_address/ip
 */

unsigned int ip6_scan(const char *s,char ip[16])
{
  unsigned int i, j;
  unsigned int len = 0;
  unsigned long u;

  char suffix[16];
  int prefixlen = 0;
  int suffixlen = 0;

 /* Always return IPv4 as IPv4-mapped IPv6 address */
  if ((i = ip4_scan(s,ip+12))) {	
    for (len = 0; len < 12; ++len) 
      ip[len] = V4mappedprefix[len];
    return i;
    if (byte_equal(ip+12,4,V4localnet)) {
      byte_copy(ip,16,V6localnet);
      return 16;
    }
  }
  byte_zero(ip,16);

  for (;;) {
    if (*s == ':') {
      len++;
      if (s[1] == ':') {        /* Found "::", skip to part 2 */
        s += 2; len++;
        break;
      }
      s++;
    }
    i = scan_xlong((char *)s,&u);
    if (!i) return 0;

    if (prefixlen == 12 && s[i] == '.') {
      /* the last 4 bytes may be written as IPv4 address */
      i = ip4_scan(s,ip+12);
      if (i) 
        return i+len;
      else
        return 0;
    }

    ip[prefixlen++] = (u >> 8);
    ip[prefixlen++] = (u & 255);
    s += i; len += i;
    if (prefixlen == 16) return len;
  }

/* part 2, after "::" */
  for (;;) {
    if (*s == ':') {
      if (suffixlen == 0) break;
      s++;
      len++;
    } else if (suffixlen != 0) break;

    i = scan_xlong((char *)s,&u);
    if (!i) {
      len--;
      break;
    }

    if (suffixlen + prefixlen <= 12 && s[i] == '.') {
      j = ip4_scan(s,suffix+suffixlen);
      if (j) {
        suffixlen += 4;
        len += j;
        break;
      } else
        prefixlen = 12 - suffixlen; /* make end-of-loop test true */
    }

    suffix[suffixlen++] = (u >> 8);
    suffix[suffixlen++] = (u & 255);
    s += i; len += i;
    if (prefixlen + suffixlen == 16) break;
  }

  for (i = 0; i < suffixlen; i++)
    ip[16 - suffixlen + i] = suffix[i];

  return len;
}

/***
 @brief ip6_scanbracket
        parse IPv6 string address enclosed in brackets
 @param input:  pointer to IPv6 address string
        output: IPv6 char array
 @return int length of ip_address (ok > 0)
 */

unsigned int ip6_scanbracket(const char *s,char ip[16])
{
  unsigned int len;

  if (*s != '[') return 0;
  len = ip6_scan(s + 1,ip);
  if (!len) return 0;
  if (s[len + 1] != ']') return 0;
  return len + 2;
}

/**
 @brief ip6_ifscan
        parse compactified IPv6 address string 
        concatinated with the interface name: fe80::1%eth0
 @param input:  pointer to IPv6 address string
        output: IPv6 char array, stralloc interface_name
 @return int length of ip6_address/ip
 */

unsigned int ip6_ifscan(char *s,char ip[16],stralloc *ifname)
{
  int i; 
  int j = 0;
  int k = 0; 
  if (!stralloc_copys(ifname,"0")) return 0;

  if ((j = str_chr(s,'%'))) {
    if ((i = str_chr(s+j+1,' '))) k = i;
    else if ((i = str_chr(s+j+1,'\n'))) k = i;
    else if ((i = str_chr(s+j+1,'\t'))) k = i;
    if (k) s[j+k+1] = '\0'; /* input might contain trailing chars */
    if (!stralloc_copys(ifname,s+j+1)) return 0;
    s[j] = 0;
  }
  if (!stralloc_0(ifname)) return 0;

  return ip6_scan(s,ip);
}

/**
 @brief ip6_cidr
        parse compactified IPv6 address string
        concatinated with the prefix length:  fe80::1/64
 @param input:  pointer to IPv6 address string
        output: IPv6 char array, long plen 
 @return int length of ip6_address/ip
 */

unsigned int ip6_cidr(char *s,char ip[16],unsigned long *plen)
{
  unsigned int j = 0;
  *plen = 128UL;

  j = str_chr(s,'/');
  if (s[j] == '/') {
     s[j] = 0;
     j = scan_ulong(s+j+1,plen);
  }
  return ip6_scan((const char *)s,ip);
}

/**
 @brief ip6_bytestring
        parse IPv6 address and represent as char string with length prefix
 @param input:  IPv6 char array, prefix length
        output: pointer to stralloc bit string;
 @return 0, if ok; 1, memory shortage
 */

unsigned int ip6_bytestring(stralloc *ipstring,char ip[16],int prefix)
{
  int i, j;
  unsigned char lowbyte, highbyte;

  ipstring->len = 0;
  if (!stralloc_copys(ipstring,"")) return 1;
  if (!stralloc_readyplus(ipstring,128)) return 1;

  for (i = 0; i < 16; i++) {
    lowbyte = (unsigned char) (ip[i]) & 0x0f;
    highbyte = (unsigned char) (ip[i] >> 4) & 0x0f;

    for (j = 3; j >= 0; j--) {
      if (highbyte & (1<<j)) {
        if (!stralloc_cats(ipstring,"1")) return 1;
      } else {
        if (!stralloc_cats(ipstring,"0")) return 1;
      }
      if (prefix == 0) {
        if (!stralloc_0(ipstring)) return 1;
        else return 0;
      }
      prefix--;
    }
    for (j = 3; j >= 0; j--) {
      if (lowbyte & (1<<j)) {
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
