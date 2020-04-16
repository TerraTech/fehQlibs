#include <stdint.h>

/**
	@file uint_t.h
	@author djb, kp, feh
	@source qmail, djbdns6
	@brief additional types and pack routines
	@comment define basic integer types and size through <stdint.h>
*/

#ifndef UINT16_H
#define UINT16_H

typedef uint16_t uint16;

extern void uint16_pack(char *,uint16);
extern void uint16_pack_big(char *,uint16);
extern void uint16_unpack(char *,uint16 *);
extern void uint16_unpack_big(char *,uint16 *);
#endif

#ifndef UINT32_H
#define UINT32_H

typedef uint32_t uint32;

extern void uint32_pack(char *,uint32);
extern void uint32_pack_big(char *,uint32);
extern void uint32_unpack(char *,uint32 *);
extern void uint32_unpack_big(char *,uint32 *);
#endif

#ifndef UINT64_H
#define UINT64_H

#ifdef HAS_UINT64_H
typedef uint64_t uint64;
#else
typedef unsigned long long uint64;
#endif

extern void uint64_pack(char *,uint64);
extern void uint64_pack_big(char *,uint64);
extern void uint64_unpack(char *,uint64 *);
extern void uint64_unpack_big(char *,uint64 *);
#endif

#ifndef UINT128_H
#define UINT128_H

/* uint128 used for native IPv6 address presentation */

struct uint128_t
{
    uint64_t hi; /* routing area */
    uint64_t lo; /* local area */
};

typedef struct uint128_t uint128;

extern void uint128_pack(char *,uint128);
extern void uint128_pack_big(char *,uint128);
extern void uint128_unpack(char *,uint128 *);
extern void uint128_unpack_big(char *,uint128 *);
#endif
