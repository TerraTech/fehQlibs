#ifndef STR_H
#define STR_H

/*
 *  Revision 20170918, Kai Peter
 *  - added 'str_copyb()', thanks Erwin Hoffmann
 *  Revision 20170501, Kai Peter
 *  - added '*str_append' and 'str_cat'
*/

extern unsigned int str_copy(char *,const char *);
extern unsigned int str_copyb(char *,const char *,unsigned int);
extern int str_diff(const char *,const char *);
extern int str_diffn(const char *,const char *,unsigned int);
//extern unsigned int str_len(char *);  // --> this produces lot of warnings !!!
extern unsigned int str_len();
extern unsigned int str_chr(const char *,int);
extern unsigned int str_rchr(const char *,int);
extern int str_start(const char *,const char *);
extern char *str_append(char *out,const char *s);

#define str_equal(s,t) (!str_diff((s),(t)))
#define str_cat(s,t) str_append(s,t)

#endif
