#include <sys/types.h>
#include <fcntl.h>
#include "open.h"

/**
	@file open.c
	@author djb
	@source qmail
	@brief open a file
*/

int open_append(char *fn)
{ return open(fn,O_WRONLY | O_NDELAY | O_APPEND | O_CREAT,0600); }

int open_excl(char *fn)
{ return open(fn,O_WRONLY | O_EXCL | O_CREAT,0644); }

int open_read(char *fn)
{ return open(fn,O_RDONLY | O_NDELAY); }

int open_trunc(char *fn)
{ return open(fn,O_WRONLY | O_NDELAY | O_TRUNC | O_CREAT,0644); }

int open_write(char *fn)
{ return open(fn,O_WRONLY | O_NDELAY); }
