#include <stdlib.h>
#include "byte.h"
#include "str.h"
#include "stralloc.h"
#include "alloc.h"

/**
	@file stralloc.c
	@author djb
	@source qmail, ucspi-tcp
	@brief genious dynamic string handling
*/

int stralloc_starts(stralloc *sa,const char *s) 
{
  int len;
  len = str_len(s);
  return (sa->len >= len) && byte_equal(s,len,sa->s);
}

int stralloc_cat(stralloc *sato,stralloc *safrom) 
{
  return stralloc_catb(sato,safrom->s,safrom->len);
}

int stralloc_catb(stralloc *sa,const char *s,unsigned int n) 
{
  if (!sa->s) return stralloc_copyb(sa,s,n);
  if (!stralloc_readyplus(sa,n + 1)) return 0;
  byte_copy(sa->s + sa->len,n,s);
  sa->len += n;
  sa->s[sa->len] = 'Z'; /* ``offensive programming'' */
  return 1;
}

int stralloc_cats(stralloc *sa,const char *s) 
{
  return stralloc_catb(sa,s,str_len(s));
}

int stralloc_copy(stralloc *sato,stralloc *safrom) 
{
  return stralloc_copyb(sato,safrom->s,safrom->len);
}


int stralloc_ready(stralloc *sa,size_t len) 
{
  register size_t wanted = len+(len>>3)+30; /* heuristic from djb */
  if (wanted<len) wanted = len;
  if (!sa->s || sa->a<len) {
    register char* tmp;
    if (!(tmp = realloc(sa->s,wanted)))    // !!! needs stdlib (realloc)
      return 0;
    sa->a = wanted;
    sa->s = tmp;
  }
  return 1;
}

int stralloc_readyplus(stralloc *sa,size_t len) 
{
  if (sa->s) {
    if (sa->len + len < len) return 0;  /* catch integer overflow */
    return stralloc_ready(sa,sa->len+len);
  } else
    return stralloc_ready(sa,len);
}

int stralloc_copyb(stralloc *sa,const char *s,unsigned int n) 
{
  if (!stralloc_ready(sa,n + 1)) return 0;
  byte_copy(sa->s,n,s);
  sa->len = n;
  sa->s[n] = 'Z'; /* ``offensive programming'' */
  return 1;
}

int stralloc_copys(stralloc *sa,const char *s) 
{
  return stralloc_copyb(sa,s,str_len(s));
}

int stralloc_catulong0(stralloc *sa,unsigned long u,unsigned int n)
{
  unsigned int len;
  unsigned long q;
  char *s;

  len = 1;
  q = u;
  while (q > 9) { ++len; q /= 10; }
  if (len < n) len = n;

  if (!stralloc_readyplus(sa,len)) return 0;
  s = sa->s + sa->len;
  sa->len += len;
  while (len) { s[--len] = '0' + (u % 10); u /= 10; }

  return 1;
}

int stralloc_catlong0(stralloc *sa,long l,unsigned int n)
{
  if (l < 0) {
    if (!stralloc_append(sa,"-")) return 0;
    l = -l;
  }
  return stralloc_catulong0(sa,l,n);
}

int stralloc_append(stralloc *sa,const char *in) 
{
  if (stralloc_readyplus(sa,1)) {
    sa->s[sa->len] = *in;
    ++sa->len;
    return 1;
  }
  return 0;
}
