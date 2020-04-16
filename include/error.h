#ifndef ERROR_H
#define ERROR_H

#include <errno.h>

#ifndef EPROTO             /* OpenBSD compat */
#define EPROTO EINTR
#endif

#define error_str(i) errstr(i)
extern char *error_str(int);

/* Exception handling notes: 
   (1) system errors RECEIVED according to <errno.h> during operation and handed over
   (2) application errors DEFINED internally and ennumerated alongside with <errno.h>
*/

/* djb backwards compatibility - deprecated form of system errors */
                                          /* Comparison of error codes and constants:
                                             intern   Linux  FreeBSD  OmniOS       */
#define error_intr          EINTR         /*    -1       4       4       4    */
#define error_nomem         ENOMEM        /*    -2      12      12      12    */
#define error_noent         ENOENT        /*    -3       2       2       2    */
#define error_txtbsy        ETXTBSY       /*    -4      26      26      26    */
#define error_io            EIO           /*    -5       5       5       5    */
#define error_exist         EEXIST        /*    -6      17      17      17    */
#define error_timeout       ETIMEDOUT     /*    -7     110      60     145    */
#define error_inprogress    EINPROGRESS   /*    -8     115      36     160    */
#define error_wouldblock    EWOULDBLOCK   /*    -9    EAGAIN  EAGAIN  EAGAIN  */
#define error_again         EAGAIN        /*   -10      11      35      11    */
#define error_pipe          EPIPE         /*   -11      32      32      32    */
#define error_perm          EPERM         /*   -12       1       1       1    */
#define error_acces         EACCES        /*   -13      13      13      13    */
#define error_nodevice      ENODEV        /*   -14      (6)     (6)     19    */
#define error_proto         EPROTO        /*   -15      71      92      71    */
#define error_isdir         EISDIR        /*   -16      21      21      21    */
#define error_connrefused   ECONNREFUSED  /*   -17     111      61     146    */
//extern int error_notdir;                /*   -18      20      20      20    */
#define error_rofs          EROFS         /*   -19      30      30      30    */
#define error_connreset     ECONNRESET    /*   -20     104      54     131    */

/* djb uses some internal application error and class definitions -- revised (feh) */
#define CAT     -10   /* raw message w/o terminating \n */
#define LOG     -90   /* generic logging */
#define INFO    -91   /* named logging */
#define TEMP    -97   /* (triggered) temporay alert condition */
#define ALERT   -98   /* (triggered) alert condition */
#define WARN    -99   /* exception condition */
#define ESOFT  -100   /* soft error, reversed negative */
#define EHARD  -111   /* hard error, reversed negative */
#define USAGE   100   /* usage error on call -- explicit usage() */
#define SYNTAX  101   /* usage/syntax error on call -- explicit syntaxerror() */
#define DROP    110   /* connection dropped -- explicit dropped() */
#define FATAL   111   /* internal error -- all */
#define ERROR   112   /* application error */

#endif
