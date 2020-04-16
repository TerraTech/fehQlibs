#ifndef STR_H
#define STR_H

/*
 *  Revision 20170918, Kai Peter
 *  - added 'str_copyb()', thanks Erwin Hoffmann
 *  Revision 20170501, Kai Peter
 *  - added '*str_append' and 'str_cat'
*/

extern unsigned int str_copy(char *,char *);
extern unsigned int str_copyb(char *,char *,unsigned int);
extern int str_diff(char *,char *);
extern int str_diffn(char *,char *,unsigned int);
//extern unsigned int str_len(char *);  // --> this produces lot of warnings !!!
extern unsigned int str_len();
extern unsigned int str_chr(char *,int);
extern unsigned int str_rchr(char *,int);
extern int str_start(char *,char *);
extern char *str_append(char *out,char *s);

#define str_equal(s,t) (!str_diff((s),(t)))
#define str_cat(s,t) str_append(s,t)

#endif
