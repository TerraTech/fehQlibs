#ifndef PATHEXEC_H
#define PATHEXEC_H
#include "stralloc.h"

extern void pathexec_run(char *,char **,char **);
extern int pathexec_env(const char *,const char *);
extern int pathexec_multienv(stralloc *);
extern void pathexec(char **);

#endif
