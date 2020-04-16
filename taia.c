#include <sys/types.h>
#include <sys/time.h>
#include "taia.h"

/**
	@file taia.c
	@author djb
	@source qmail
	@brief 'tai' attosecond time handling
*/

/* XXX: breaks tai encapsulation */

void taia_add(struct taia *t,struct taia *u,struct taia *v)
{
  t->sec.x = u->sec.x + v->sec.x;
  t->nano = u->nano + v->nano;
  t->atto = u->atto + v->atto;
  if (t->atto > 999999999UL) {
    t->atto -= 1000000000UL;
    ++t->nano;
  }
  if (t->nano > 999999999UL) {
    t->nano -= 1000000000UL;
    ++t->sec.x;
  }
}

double taia_approx(struct taia *t)
{
  return tai_approx(&t->sec) + taia_frac(t);
}

double taia_frac(struct taia *t)
{
  return (t->atto * 0.000000001 + t->nano) * 0.000000001;
}

int taia_less(struct taia *t,struct taia *u)
{
  if (t->sec.x < u->sec.x) return 1;
  if (t->sec.x > u->sec.x) return 0;
  if (t->nano < u->nano) return 1;
  if (t->nano > u->nano) return 0;
  return t->atto < u->atto;
}

int taia_now(struct taia *t)
{
  struct timeval now;
  if (gettimeofday(&now,(struct timezone *) 0) == 0) {
    tai_unix(&t->sec,now.tv_sec);
    t->nano = 1000 * now.tv_usec + 500;
    t->atto = 0;
    return 0;
  }
  t->nano = 0;
  t->atto = 0;
  return -1;
}

void taia_pack(char *s,struct taia *t)
{
  unsigned long x;

  tai_pack(s,&t->sec);
  s += 8;

  x = t->atto;
  s[7] = x & 255; x >>= 8;
  s[6] = x & 255; x >>= 8;
  s[5] = x & 255; x >>= 8;
  s[4] = x;
  x = t->nano;
  s[3] = x & 255; x >>= 8;
  s[2] = x & 255; x >>= 8;
  s[1] = x & 255; x >>= 8;
  s[0] = x;
}

void taia_sub(struct taia *t,struct taia *u,struct taia *v)
{
  unsigned long unano = u->nano;
  unsigned long uatto = u->atto;

  t->sec.x = u->sec.x - v->sec.x;
  t->nano = unano - v->nano;
  t->atto = uatto - v->atto;
  if (t->atto > uatto) {
    t->atto += 1000000000UL;
    --t->nano;
  }
  if (t->nano > unano) {
    t->nano += 1000000000UL;
    --t->sec.x;
  }
}

void taia_uint(struct taia *t,unsigned int s)
{
  t->sec.x = s;
  t->nano = 0;
  t->atto = 0;
}
