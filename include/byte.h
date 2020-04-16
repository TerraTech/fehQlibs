#ifndef BYTE_H
#define BYTE_H

/**
	@file byte.h	
	@author djb, feh
	@source s/qmail
	@comment no declaration of argument types; too many compiler errors
*/

extern unsigned int byte_chr();
extern unsigned int byte_rchr();
extern void byte_copy();
extern void byte_copyr();
extern int byte_diff();
extern void byte_zero();
extern void byte_fill();

#define byte_equal(s,n,t) (!byte_diff((s),(n),(t)))

#endif
