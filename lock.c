#include <unistd.h>
#include <sys/types.h>
#include <sys/file.h>
#include <fcntl.h>
#include "lock.h"

/**
	@file lock.c
	@author djb
	@source qmail
	@brief locking of resources
*/

#ifdef HASFLOCK
int lock_ex(int fd) { return flock(fd,LOCK_EX); }
int lock_exnb(int fd) { return flock(fd,LOCK_EX | LOCK_NB); }
int lock_un(int fd) { return flock(fd,LOCK_UN); }
#else
int lock_ex(int fd) { return lockf(fd,1,0); }
int lock_exnb(int fd) { return lockf(fd,2,0); }
int lock_un(int fd) { return lockf(fd,0,0); }
#endif
