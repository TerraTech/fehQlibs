#include <time.h>
#include "tai.h"

/**
	@file tai.c
	@author djb
	@source qmail
	@brief 'temps atomic' time handling
*/

void tai_add(struct tai *t,const struct tai *u,const struct tai *v)
{
  t->x = u->x + v->x;
}

void tai_now(struct tai *t)
{
  tai_unix(t,time((time_t *) 0));
}

void tai_pack(char *s,const struct tai *t)
{
  uint64 x;

  x = t->x;
  s[7] = (char)x; x >>= 8;
  s[6] = (char)x; x >>= 8;
  s[5] = (char)x; x >>= 8;
  s[4] = (char)x; x >>= 8;
  s[3] = (char)x; x >>= 8;
  s[2] = (char)x; x >>= 8;
  s[1] = (char)x; x >>= 8;
  s[0] = (char)x;
}

void tai_sub(struct tai *t,const struct tai *u,const struct tai *v)
{
  t->x = u->x - v->x;
}

void tai_uint(struct tai *t,unsigned int u)
{
  t->x = u;
}

void tai_unpack(const char *s,struct tai *t)
{
  uint64 x;

  x = (unsigned char) s[0];
  x <<= 8; x += (unsigned char) s[1];
  x <<= 8; x += (unsigned char) s[2];
  x <<= 8; x += (unsigned char) s[3];
  x <<= 8; x += (unsigned char) s[4];
  x <<= 8; x += (unsigned char) s[5];
  x <<= 8; x += (unsigned char) s[6];
  x <<= 8; x += (unsigned char) s[7];
  t->x = x;
}
