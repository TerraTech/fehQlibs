#include <sys/types.h>
#include <unistd.h>
#include <grp.h>
//#include "hasshsgr.h"
#include "prot.h"

/**
	@file prot.c
	@author djb
	@source qmail
	@brief setting up uid an gid for OS (short group was for ancient solaris)
*/

/* XXX: there are more portability problems here waiting to leap out at me */

int prot_gid(int gid)
{
//#ifdef HASSHORTSETGROUPS
//  short x[2];
//  x[0] = gid; x[1] = 73; /* catch errors */
//  if (setgroups(1,x) == -1) return -1;
//#else
  if (setgroups(1,(gid_t *)&gid) == -1) return -1;
//#endif
  return setgid(gid); /* _should_ be redundant, but on some systems it isn't */
}

int prot_uid(int uid) 
{
  return setuid(uid);
}
