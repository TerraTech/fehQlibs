#include <unistd.h>
#include <stdarg.h>
#include <errno.h>
#include "buffer.h"
#include "fmt.h"
#include "str.h" 
#include "stralloc.h" 
#include "logmsg.h"

/**
	@file logmsg.c
	@author kp, feh
	@source qlibs
	@brief unified system and error message handling
*/

#define WHO "logmsg"

char *build_log_msg(const char *x[])
{
  stralloc sa = {0};
  stralloc_copys(&sa,"");  

  while(*x) { if (!stralloc_cats(&sa,*x++)) err_sys(WHO,errno); } /* concatenate *x */
  
  if (!stralloc_0(&sa)) err_sys(WHO,errno);
  return(sa.s);
}

void logmsg(const char *who,int ecode,unsigned int class,const char *msg)
{
  char strnum[FMT_ULONG];
  char *codestr = "";
  char *classstr = "";
  char *errmsg = "";

  errno = 0;   /* re-initialize errno, value is in 'code' now */

/* Part 1: obtain the (error) code -- perhaps received from OS */

  if (ecode != 0) {
    codestr = "";
    if (ecode < 0) {           // check for negative error codes 
      ecode = (ecode^-1) + 1;
      codestr = "-";
    }
    strnum[fmt_ulong(strnum,ecode)] = 0;    /* format for output */
    char *temp = strnum;
    codestr = str_cat(codestr,temp);
  }

/* Part 2: behavioral on error */

  switch (class) {
    case ERROR:  classstr = "error: ";    break;   // info + exit
    case FATAL:  classstr = "fatal: ";    break;   // info + exit 
    case DROP:   classstr = "drop: ";     break;   // info + next call/iteration
    case ALERT:  classstr = "alert: ";    break;   // info + next statement
    case WARN:   classstr = "warning: ";  break;   // info + next statement
    case INFO:   classstr = "info: ";     break;   // info + continue
    case SYNTAX: classstr = "syntax: ";   break;   // info + exit
    case USAGE:  classstr = "usage: ";    break;   // info + exit
    case TEMP:   classstr = "temp: ";    break;   // info + exit
    case CAT:    classstr = "";	          break;   // info w/o \n
    default: 
      class = LOG; classstr =  "";        break;   // custom info + continue
  }

/* Part 3: get system error message */

  if (class == FATAL || class == DROP)
    errmsg = error_str(errno);

/* Part 4: construct log message: Source: Class (Ecode) Message: Errmsg */

  buffer_puts(buffer_2,who);
  buffer_puts(buffer_2,": ");
  buffer_puts(buffer_2,classstr);
  if (class == FATAL || class == DROP || class == ERROR) {
    buffer_puts(buffer_2,"(");
    buffer_puts(buffer_2,codestr);
    buffer_puts(buffer_2,") ");
  }
  buffer_puts(buffer_2,msg);
  if (errno)  {
    buffer_puts(buffer_2,": ");
    buffer_puts(buffer_2,errmsg);
  }
  if (class != CAT) {
    buffer_puts(buffer_2,"\n");
    buffer_flush(buffer_2);
  }

  if (class == USAGE) _exit(USAGE); 
  if (class == SYNTAX) _exit(SYNTAX); 
  if (class == FATAL || class == ERROR) _exit(ecode);
}
