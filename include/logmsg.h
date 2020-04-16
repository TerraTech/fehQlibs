#ifndef LOGMSG_H
#define LOGMSG_H

#include <errno.h>
#include <stdlib.h>
#include "error.h"

extern void logmsg(char *who, int ecode, unsigned int class, char *msg);

/* useful combinations of params */
/* class: FATAL (hard) - system error */
#define err_sys(w,e) logmsg(w,e,FATAL,"")
#define err_sys_plus(w,e,m) logmsg(w,e,FATAL,m)
/* class: WARN (application) - temporary error */
#define err_tmp(w,e,m) logmsg(w,e,WARN,m)
#define err_tmp_plus(w,e,m) logmsg(w,e,WARN,m)
/* class: facultative ('int'ernal definition */
#define err_int(w,e,c) logmsg(w,e,c,"")
#define err_int_plus(w,e,c,m) logmsg(w,e,c,m)
/* log messages */
/* #define log(w,m) logmsg(w,0,LOG,m) // obsoleted by */
#define log_who(w,m) logmsg(w,0,LOG,m)
#define log_anon(m) logmsg("",0,LOG,m)
#define log_cat(n) logmsg("",0,CAT,m)

/* build log message from multiple partial strings */
extern char *build_log_msg(char *[]);
#define B(...) build_log_msg((char *[]){__VA_ARGS__,NULL}) // K/R sect. 7.3

#endif
