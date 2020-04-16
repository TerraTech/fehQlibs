#include <signal.h>
#include "sig.h"

/**
	@file sig.c
	@author djb
	@source qmail
	@brief signal handling functions
*/

void sig_alarmblock() { sig_block(SIGALRM); }
void sig_alarmunblock() { sig_unblock(SIGALRM); }
void sig_alarmcatch(f) void (*f)(); { sig_catch(SIGALRM,f); }
void sig_alarmdefault() { sig_catch(SIGALRM,SIG_DFL); }
int sig_alarm = SIGALRM;

void sig_block(int sig) 
{
  sigset_t ss;
  sigemptyset(&ss);
  sigaddset(&ss,sig);
  sigprocmask(SIG_BLOCK,&ss,(sigset_t *) 0);
}

void sig_unblock(int sig) 
{
  sigset_t ss;
  sigemptyset(&ss);
  sigaddset(&ss,sig);
  sigprocmask(SIG_UNBLOCK,&ss,(sigset_t *) 0);
}

void sig_blocknone() 
{
  sigset_t ss;
  sigemptyset(&ss);
  sigprocmask(SIG_SETMASK,&ss,(sigset_t *) 0);
}

void sig_catch(int sig,void (*f)()) 
{
  struct sigaction sa;
  sa.sa_handler = f;
  sa.sa_flags = 0;
  sigemptyset(&sa.sa_mask);
  sigaction(sig,&sa,(struct sigaction *) 0);
}

void sig_pause() 
{
  sigset_t ss;
  sigemptyset(&ss);
  sigsuspend(&ss);
}

void sig_pipeignore() { sig_catch(SIGPIPE,SIG_IGN); }
void sig_pipedefault() { sig_catch(SIGPIPE,SIG_DFL); }
int sig_pipe = SIGPIPE;

void sig_childblock() { sig_block(SIGCHLD); }
void sig_childunblock() { sig_unblock(SIGCHLD); }
void sig_childcatch(f) void (*f)(); { sig_catch(SIGCHLD,f); }
void sig_childdefault() { sig_catch(SIGCHLD,SIG_DFL); }
int sig_child = SIGCHLD;

void sig_hangupblock() { sig_block(SIGHUP); }
void sig_hangupunblock() { sig_unblock(SIGHUP); }
void sig_hangupcatch(f) void (*f)(); { sig_catch(SIGHUP,f); }
void sig_hangupdefault() { sig_catch(SIGHUP,SIG_DFL); }
int sig_hangup = SIGHUP;

void sig_termblock() { sig_block(SIGTERM); }
void sig_termunblock() { sig_unblock(SIGTERM); }
void sig_termcatch(f) void (*f)(); { sig_catch(SIGTERM,f); }
void sig_termdefault() { sig_catch(SIGTERM,SIG_DFL); }
int sig_term = SIGTERM;

void sig_bugcatch(void (*f)()) 
{
  sig_catch(SIGILL,f);
  sig_catch(SIGABRT,f);
  sig_catch(SIGFPE,f);
  sig_catch(SIGBUS,f);
  sig_catch(SIGSEGV,f);
#ifdef SIGSYS
  sig_catch(SIGSYS,f);
#endif
#ifdef SIGEMT
  sig_catch(SIGEMT,f);
#endif
}
void (*sig_defaulthandler)() = SIG_DFL;

void sig_miscignore() 
{
  sig_catch(SIGVTALRM,SIG_IGN);
  sig_catch(SIGPROF,SIG_IGN);
  sig_catch(SIGQUIT,SIG_IGN);
  sig_catch(SIGINT,SIG_IGN);
  sig_catch(SIGHUP,SIG_IGN);
#ifdef SIGXCPU
  sig_catch(SIGXCPU,SIG_IGN);
#endif
#ifdef SIGXFSZ
  sig_catch(SIGXFSZ,SIG_IGN);
#endif
}
void (*sig_ignorehandler)() = SIG_IGN;

int sig_cont = SIGCONT;
