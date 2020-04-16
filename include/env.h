#ifndef ENV_H
#define ENV_H

/*
 *  Revision 20171220, Erwin Hoffmann
 *  - removed variable names
 *  Revision 20160628, Kai Peter
 *  - updated code (*env_get) like in ucspi-tcp-0.88
 *  - commented out 'env_pick' and 'env_clear'
*/

extern char **environ;

extern int env_isinit;
extern int env_init();

extern /*@null@*/char *env_get(char *);
extern int env_put(char *,char *);
extern int env_puts(char *);
extern int env_unset(char *);
extern char *env_pick();
extern void env_clear();

extern char *env_findeq(char *);

#define env_put2 env_put /* backwards compatibility */

#endif
