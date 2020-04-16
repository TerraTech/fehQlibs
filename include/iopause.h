#ifndef IOPAUSE_H
#define IOPAUSE_H

#define IOPAUSE_POLL

#include <sys/types.h>
#ifdef HAS_POLL_H
#include <poll.h>

typedef struct pollfd iopause_fd;
#define IOPAUSE_READ POLLIN
#define IOPAUSE_WRITE POLLOUT
#else
typedef struct {
  int fd;
  short events;
  short revents;
} iopause_fd;
#define IOPAUSE_READ 1
#define IOPAUSE_WRITE 4
#endif

#include "taia.h"

extern int iopause(iopause_fd *,unsigned int,struct taia *,struct taia *);

#endif
