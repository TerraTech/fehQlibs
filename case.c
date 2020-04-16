#include "case.h"
#include "str.h"

/**
	@file case.c
	@author djb
	@brief	string comparison and helper functions; case insensitive
*/

int case_diffb(register char *s,unsigned int len,register char *t)
{
  register unsigned char x;
  register unsigned char y;

  while (len > 0) {
    --len;
    x = *s++ - 'A';
    if (x <= 'Z' - 'A') x += 'a'; else x += 'A';
    y = *t++ - 'A';
    if (y <= 'Z' - 'A') y += 'a'; else y += 'A';
    if (x != y)
      return ((int)(unsigned int) x) - ((int)(unsigned int) y);
  }
  return 0;
}

int case_diffs(register char *s,register char *t)
{
  register unsigned char x;
  register unsigned char y;

  for (;;) {
    x = *s++ - 'A';
    if (x <= 'Z' - 'A') x += 'a'; else x += 'A';
    y = *t++ - 'A';
    if (y <= 'Z' - 'A') y += 'a'; else y += 'A';
    if (x != y) break;
    if (!x) break;
  }
  return ((int)(unsigned int) x) - ((int)(unsigned int) y);
}

int case_diffrs(register char *s,register char *t)
{
  register unsigned char x = 0;
  register unsigned char y = 0;
  unsigned int lens = str_len(s);
  unsigned int lent = str_len(t);

  while (lens > 0 && lent > 0) {
    x = s[--lens] - 'A';
    if (x <= 'Z' - 'A') x += 'a'; else x += 'A';
    y = t[--lent] - 'A';
    if (y <= 'Z' - 'A') y += 'a'; else y += 'A';
    if (x != y) break;
    if (!x) break;
    if (!y) break;
  }
  return ((int)(unsigned int) x) - ((int)(unsigned int) y);
}

void case_lowerb(char *s,unsigned int len)
{
  unsigned char x;
  while (len > 0) {
    --len;
    x = *s - 'A';
    if (x <= 'Z' - 'A') *s = x + 'a';
    ++s;
  }
}

void case_lowers(char *s)
{
  unsigned char x;
  while ((x = *s)) {
    x -= 'A';
    if (x <= 'Z' - 'A') *s = x + 'a';
    ++s;
  }
}

int case_startb(register char *s,unsigned int len,register char *t)
{
  register unsigned char x;
  register unsigned char y;

  for (;;) {
    y = *t++ - 'A';
    if (y <= 'Z' - 'A') y += 'a'; else y += 'A';
    if (!y) return 1;
    if (!len) return 0;
    --len;
    x = *s++ - 'A';
    if (x <= 'Z' - 'A') x += 'a'; else x += 'A';
    if (x != y) return 0;
  }
}

int case_starts(register char *s,register char *t)
{
  register unsigned char x;
  register unsigned char y;

  for (;;) {
    x = *s++ - 'A';
    if (x <= 'Z' - 'A') x += 'a'; else x += 'A';
    y = *t++ - 'A';
    if (y <= 'Z' - 'A') y += 'a'; else y += 'A';
    if (!y) return 1;
    if (x != y) return 0;
  }
}
