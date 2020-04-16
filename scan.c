#include "scan.h"

/**
	@file scan.c
	@author djb
	@source qmail, ucspi-tcp
	@brief scanning/conversion of strings to different variable types
*/

static long int fromhex(unsigned char c) 
{
  if (c>='0' && c<='9')
    return c-'0';
  else if (c>='A' && c<='F')
    return c-'A'+10;
  else if (c>='a' && c<='f')
    return c-'a'+10;
  return -1;
}

unsigned int scan_0x(register const char *s,register unsigned int *u)
{
  register unsigned int pos = 0;
  register unsigned long result = 0;
  register long int c;

  while ((c = fromhex((unsigned char) (s[pos]))) >= 0) {
    result = (result << 4) + c;
    ++pos;
  }
  *u = result;
  return pos;
}

unsigned int scan_8long(register const char *s,register unsigned long *u)
{
  register unsigned int pos = 0;
  register unsigned long result = 0;
  register unsigned long c;

  while ((c = (unsigned long) (unsigned char) (s[pos] - '0')) < 8) {
    result = result * 8 + c;
    ++pos; 
  }
  *u = result;
  return pos;
}

unsigned int scan_uint(register const char *s,register unsigned int *u)
{
  register unsigned int pos; 
  unsigned long result;

  pos = scan_ulong(s,&result);
  *u = result; 
  return pos;
}

unsigned int scan_ulong(register const char *s,register unsigned long *u)
{
  register unsigned int pos = 0;
  register unsigned long result = 0;
  register unsigned long c;

  while ((c = (unsigned long) (unsigned char) (s[pos] - '0')) < 10) {
    result = result * 10 + c;
    ++pos; 
  }
  *u = result;
  return pos;
}

unsigned int scan_xlong(const char *s,unsigned long *u)
{
  register const char *t = s;
  register int l = 0;
  register unsigned char c;

  while ((c = fromhex(*t)) < 16) {
    l = (l<<4)+c;
    ++t;
  }
  *u=l;
  return t-s;
}

unsigned int scan_xint(const char *s,unsigned int *i)
{
  register const char *t = s;
  register unsigned int l = 0;
  register unsigned char c;

  while ((l >> (sizeof(l)*8 - 4)) == 0 
      && (c = (unsigned char)fromhex((unsigned char)*t))<16) {
    l= (l << 4) + c;
    ++t;
  }
  *i = l;
  return (unsigned int)(t-s);
}
