#include "buffer.h"
#include "getoptb.h"

/**
	@file getoptb.c
	@author djb
	@source ucspi-tcp
	@brief 'getopt' version w/o stdlib
*/

#define optind subgetoptind
#define optproblem subgetoptproblem

int opterr = 1;
char *optprogname = 0;

int getopt(int argc,char **argv,char *opts)
{
  int c;
  char *s;

  if (!optprogname) {
    optprogname = *argv;
    if (!optprogname) optprogname = "";
    for (s = optprogname;*s;++s) if (*s == '/') optprogname = s + 1;
  }
  c = subgetopt(argc,argv,opts);
  if (opterr)
    if (c == '?') {
      char chp[2]; chp[0] = optproblem; chp[1] = '\n';
      buffer_puts(buffer_2,optprogname);
      if (argv[optind] && (optind < argc))
        buffer_puts(buffer_2,": illegal option -- ");
      else
        buffer_puts(buffer_2,": option requires an argument -- ");
      buffer_put(buffer_2,chp,2);
      buffer_flush(buffer_2);
    }
  return c;
}

#define optpos subgetoptpos
#define optarg subgetoptarg
#define optdone subgetoptdone

int optind = 1;
int optpos = 0;
char *optarg = 0;
int optproblem = 0;
int optdone = SUBGETOPTDONE;

int subgetopt(int argc,char **argv,char *opts)
{
  int c;
  char *s;

  optarg = 0;
  if (!argv || (optind >= argc) || !argv[optind]) return optdone;
  if (optpos && !argv[optind][optpos]) {
    ++optind;
    optpos = 0;
    if ((optind >= argc) || !argv[optind]) return optdone;
  }
  if (!optpos) {
    if (argv[optind][0] != '-') return optdone;
    ++optpos;
    c = argv[optind][1];
    if ((c == '-') || (c == 0)) {
      if (c) ++optind;
      optpos = 0;
      return optdone;
    }
    /* otherwise c is reassigned below */
  }
  c = argv[optind][optpos];
  ++optpos;
  s = opts;
  while (*s) {
    if (c == *s) {
      if (s[1] == ':') {
        optarg = argv[optind] + optpos;
        ++optind;
        optpos = 0;
        if (!*optarg) {
          optarg = argv[optind];
          if ((optind >= argc) || !optarg) { /* argument past end */
            optproblem = c;
            return '?';
          }
      ++optind;
        }
      }
      return c;
    }
    ++s;
    if (*s == ':') ++s;
  }
  optproblem = c;
  return '?';
}
