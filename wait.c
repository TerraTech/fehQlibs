#include <sys/types.h>
#include <sys/wait.h>
#include "logmsg.h"

/**
	@file wait.c
	@author djb
	@source qmail
	@brief wait for forked processes
*/

int wait_nohang(int *wstat)
{
  return waitpid(-1,wstat,WNOHANG);
}

int wait_pid(int *wstat,int pid)
{
  int r;

  do
    r = waitpid(pid,wstat,0);
  while ((r == -1) && (errno == EINTR));
  return r;
}
