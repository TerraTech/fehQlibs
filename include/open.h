#ifndef OPEN_H
#define OPEN_H

/**
	@file open.h
	@author djb, feh
	@source s/qmail
*/

int open_read(char *);
int open_excl(char *);
int open_append(char *);
int open_trunc(char *);
int open_write(char *);

#endif
