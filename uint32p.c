#include "uint_t.h"

/**
	@file uint32p.c
	@author djb
	@source qmail
	@brief packing/unpacking 32 bit integer to/from char string
*/

void uint32_pack(char s[4],uint32 u) 
{
  s[0] = u & 255; u >>= 8;
  s[1] = u & 255; u >>= 8;
  s[2] = u & 255;
  s[3] = u >> 8;
}
void uint32_pack_big(char s[4],uint32 u)
{
  s[3] = u & 255; u >>= 8;
  s[2] = u & 255; u >>= 8;
  s[1] = u & 255;
  s[0] = u >> 8;
}

void uint32_unpack(char s[4],uint32 *u)
{
  uint32 result;

  result =  (unsigned char) s[3]; result <<= 8;
  result += (unsigned char) s[2]; result <<= 8;
  result += (unsigned char) s[1]; result <<= 8;
  result += (unsigned char) s[0];

  *u = result;
}

void uint32_unpack_big(char s[4],uint32 *u)
{
  uint32 result;

  result =  (unsigned char) s[0]; result <<= 8;
  result += (unsigned char) s[1]; result <<= 8;
  result += (unsigned char) s[2]; result <<= 8;
  result += (unsigned char) s[3];

  *u = result;
}
