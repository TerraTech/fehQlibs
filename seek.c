#include <sys/types.h>
#include "seek.h"

/**
	@file seek.c
	@author djb
	@source qmail
	@brief seek in an open file descritor
*/

off_t lseek(int fd,off_t offset,int whence);
int ftruncate(int fd, off_t length);

#define CUR 1 /* sigh */

seek_pos seek_cur(int fd)
{ return lseek(fd,(off_t) 0,CUR); }

#define END 2 /* sigh */

int seek_end(int fd)
{ if (lseek(fd,(off_t) 0,END) == -1) return -1; return 0; }

#define SET 0 /* sigh */

int seek_set(int fd,seek_pos pos)
{ if (lseek(fd,(off_t) pos,SET) == -1) return -1; return 0; }

int seek_trunc(int fd,seek_pos pos)
{ return ftruncate(fd,(off_t) pos); }
