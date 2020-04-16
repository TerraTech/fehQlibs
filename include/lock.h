#ifndef LOCK_H
#define LOCK_H

/**
	@file lock.h
	@author djb, feh
	@source s/qmail
*/

int lock_ex(int);
int lock_un(int);
int lock_exnb(int);

#endif
