#include <unistd.h>
#include "error.h"
#include "iopause.h"
#include "timeout.h"

/**
	@file timeout.c
	@author djb
	@source qmail
	@brief read/write timeout handling
*/

int timeoutread(int t,int fd,char *buf,int len)
{
  struct taia now;
  struct taia deadline;
  iopause_fd x;
  int r;

  taia_now(&now);
  taia_uint(&deadline,t);
  taia_add(&deadline,&now,&deadline);

  x.fd = fd;
  x.events = IOPAUSE_READ;
  for (;;) {
    taia_now(&now);
    r = iopause(&x,1,&deadline,&now);
    if (r > 0 && x.revents) break;
    if (taia_less(&deadline,&now)) {
      errno = ETIMEDOUT;
      return -1;
    }
  }
  return read(fd,buf,len);
}

int timeoutwrite(int t,int fd,char *buf,int len)
{
  struct taia now;
  struct taia deadline;
  iopause_fd x;
  int r;

  taia_now(&now);
  taia_uint(&deadline,t);
  taia_add(&deadline,&now,&deadline);

  x.fd = fd;
  x.events = IOPAUSE_WRITE;
  for (;;) {
    taia_now(&now);
    r = iopause(&x,1,&deadline,&now);
    if (r > 0 && x.revents) break;
    if (taia_less(&deadline,&now)) {
      errno = ETIMEDOUT;
      return -1;
    }
  }
  return write(fd,buf,len);
}
