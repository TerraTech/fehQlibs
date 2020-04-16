#include "str.h"
#include "stralloc.h"

/**
	@file str.c
	@author djb
	@source qmail
	@brief string handling functions
*/

unsigned int str_copyb(register char *s,register char *t,unsigned int max)
{
  register int len;

  len = 0;

  while (max-- > 0) {
    if (!(*s = *t)) return len; ++s; ++t; ++len;
    if (!(*s = *t)) return len; ++s; ++t; ++len;
    if (!(*s = *t)) return len; ++s; ++t; ++len;
    if (!(*s = *t)) return len; ++s; ++t; ++len;
  }

  return len;
}

unsigned int str_chr(register char *s,int c) 
{
  register char ch;
  register char *t;

  ch = c;
  t = s;
  for (;;) {
    if (!*t) { break; } if (*t == ch) { break; } ++t;
    if (!*t) { break; } if (*t == ch) { break; } ++t;
    if (!*t) { break; } if (*t == ch) { break; } ++t;
    if (!*t) { break; } if (*t == ch) { break; } ++t;
  }
  return t - s;
}

unsigned int str_copy(register char *s,register char *t) 
{
  register int len;

  len = 0;
  for (;;) {
    if (!(*s = *t)) { return len; } ++s; ++t; ++len; 
    if (!(*s = *t)) { return len; } ++s; ++t; ++len;
    if (!(*s = *t)) { return len; } ++s; ++t; ++len;
    if (!(*s = *t)) { return len; } ++s; ++t; ++len;
  }
}

int str_diff(register char *s,register char *t) 
{
  register char x;

  for (;;) {
    x = *s; if (x != *t) { break; } if (!x) { break; } ++s; ++t;
    x = *s; if (x != *t) { break; } if (!x) { break; } ++s; ++t;
    x = *s; if (x != *t) { break; } if (!x) { break; } ++s; ++t;
    x = *s; if (x != *t) { break; } if (!x) { break; } ++s; ++t;
  }
  return ((int)(unsigned int)(unsigned char) x)
       - ((int)(unsigned int)(unsigned char) *t);
}

int str_diffn(register char *s,register char *t,unsigned int len) 
{
  register char x;

  for (;;) {
    if (!len--) { return 0; } x = *s; if (x != *t) { break; } if (!x) { break; } ++s; ++t;
    if (!len--) { return 0; } x = *s; if (x != *t) { break; } if (!x) { break; } ++s; ++t;
    if (!len--) { return 0; } x = *s; if (x != *t) { break; } if (!x) { break; } ++s; ++t;
    if (!len--) { return 0; } x = *s; if (x != *t) { break; } if (!x) { break; } ++s; ++t;
  }
  return ((int)(unsigned int)(unsigned char) x)
       - ((int)(unsigned int)(unsigned char) *t);
}

unsigned int str_len(register char *s) 
{
  register char *t;

  t = s;
  for (;;) {
    if (!*t) { return t - s; } ++t;
    if (!*t) { return t - s; } ++t;
    if (!*t) { return t - s; } ++t;
    if (!*t) { return t - s; } ++t;
  }
}

unsigned int str_rchr(register char *s,int c) 
{
  register char ch;
  register char *t;
  register char *u;

  ch = c;
  t = s;
  u = 0;
  for (;;) {
    if (!*t) { break; } if (*t == ch) { u = t; } ++t;
    if (!*t) { break; } if (*t == ch) { u = t; } ++t;
    if (!*t) { break; } if (*t == ch) { u = t; } ++t;
    if (!*t) { break; } if (*t == ch) { u = t; } ++t;
  }
  if (!u) u = t;
  return u - s;
}

int str_start(register char *s,register char *t) 
{
  register char x;

  for (;;) {
    x = *t++; if (!x) return 1; if (x != *s++) return 0;
    x = *t++; if (!x) return 1; if (x != *s++) return 0;
    x = *t++; if (!x) return 1; if (x != *s++) return 0;
    x = *t++; if (!x) return 1; if (x != *s++) return 0;
  }
}

char *str_append(char *dest, char *s) 
{
  static stralloc sa = {0};

  stralloc_copys(&sa,dest);
  stralloc_catb(&sa,s,sizeof(s));
  return sa.s;
}
