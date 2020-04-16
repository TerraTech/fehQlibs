#include "uint_t.h"

/**
	@file uint128p.c
	@author feh, jannis
	@source djbdns6
	@brief packing/unpacking 128 bit integer to/from char string
*/
 
void uint128_pack(char s[16],uint128 u)
{
   s[0] = u.lo & 255; u.lo >>= 8;
   s[1] = u.lo & 255; u.lo >>= 8;
   s[2] = u.lo & 255; u.lo >>= 8;
   s[3] = u.lo & 255; u.lo >>= 8;
   s[4] = u.lo & 255; u.lo >>= 8;
   s[5] = u.lo & 255; u.lo >>= 8;
   s[6] = u.lo & 255; u.lo >>= 8;
   s[7] = u.lo & 255; u.lo >>= 8;

   s[8] = u.hi & 255; u.hi >>= 8;
   s[9] = u.hi & 255; u.hi >>= 8;
  s[10] = u.hi & 255; u.hi >>= 8;
  s[11] = u.hi & 255; u.hi >>= 8;
  s[12] = u.hi & 255; u.hi >>= 8;
  s[13] = u.hi & 255; u.hi >>= 8;
  s[14] = u.hi & 255; u.hi >>= 8;
  s[15] = u.hi & 255; 
}
void uint128_pack_big(char s[16],uint128 u)
{
  s[15] = u.lo & 255; u.lo >>= 8;
  s[14] = u.lo & 255; u.lo >>= 8;
  s[13] = u.lo & 255; u.lo >>= 8;
  s[12] = u.lo & 255; u.lo >>= 8;
  s[11] = u.lo & 255; u.lo >>= 8;
  s[10] = u.lo & 255; u.lo >>= 8;
   s[9] = u.lo & 255; u.lo >>= 8;
   s[8] = u.lo & 255; u.lo >>= 8;

   s[7] = u.hi & 255; u.hi >>= 8;
   s[6] = u.hi & 255; u.hi >>= 8;
   s[5] = u.hi & 255; u.hi >>= 8;
   s[4] = u.hi & 255; u.hi >>= 8;
   s[3] = u.hi & 255; u.hi >>= 8;
   s[2] = u.hi & 255; u.hi >>= 8;
   s[1] = u.hi & 255; u.hi >>= 8;
   s[0] = u.hi & 255; 
}

void uint128_unpack(char s[16],uint128 *u)
{
  uint128 result;
  result.hi = result.lo = 0ULL;

  result.hi =  (unsigned char) s[15]; result.hi <<= 8;
  result.hi += (unsigned char) s[14]; result.hi <<= 8;
  result.hi += (unsigned char) s[13]; result.hi <<= 8;
  result.hi += (unsigned char) s[12]; result.hi <<= 8;
  result.hi += (unsigned char) s[11]; result.hi <<= 8;
  result.hi += (unsigned char) s[10]; result.hi <<= 8;
  result.hi += (unsigned char)  s[9]; result.hi <<= 8;
  result.hi += (unsigned char)  s[8];   //correct

  result.lo += (unsigned char)  s[7]; result.lo <<= 8;  
  result.lo += (unsigned char)  s[6]; result.lo <<= 8;
  result.lo += (unsigned char)  s[5]; result.lo <<= 8;
  result.lo += (unsigned char)  s[4]; result.lo <<= 8;
  result.lo += (unsigned char)  s[3]; result.lo <<= 8;
  result.lo += (unsigned char)  s[2]; result.lo <<= 8;
  result.lo += (unsigned char)  s[1]; result.lo <<= 8;
  result.lo += (unsigned char)  s[0];  

  *u = result;
}
void uint128_unpack_big(char s[16],uint128 *u)
{
  uint128 result;
  result.hi = result.lo = 0ULL;

  result.hi =  (unsigned char) s[0]; result.hi <<= 8;
  result.hi += (unsigned char) s[1]; result.hi <<= 8;
  result.hi += (unsigned char) s[2]; result.hi <<= 8;
  result.hi += (unsigned char) s[3]; result.hi <<= 8;
  result.hi += (unsigned char) s[4]; result.hi <<= 8;
  result.hi += (unsigned char) s[5]; result.hi <<= 8;
  result.hi += (unsigned char) s[6]; result.hi <<= 8;
  result.hi += (unsigned char) s[7];
  
  result.lo += (unsigned char)  s[8]; result.lo <<= 8;
  result.lo += (unsigned char)  s[9]; result.lo <<= 8;
  result.lo += (unsigned char) s[10]; result.lo <<= 8;
  result.lo += (unsigned char) s[11]; result.lo <<= 8;
  result.lo += (unsigned char) s[12]; result.lo <<= 8;
  result.lo += (unsigned char) s[13]; result.lo <<= 8;
  result.lo += (unsigned char) s[14]; result.lo <<= 8;
  result.lo += (unsigned char) s[15];

  *u = result;
}
