#ifndef TIMEOUTCONN_H
#define TIMEOUTCONN_H

#include "uint_t.h"

extern int timeoutconn4(int,char *,uint16,unsigned int);
extern int timeoutconn6(int,char *,uint16,unsigned int,uint32);
extern int timeoutconn(int,char *,uint16,unsigned int,uint32);

#endif
