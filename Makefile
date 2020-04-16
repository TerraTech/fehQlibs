# Makefile for qlibs

SHELL=/bin/sh

# static flags - DO NOT EDIT!
CCFLAGS=-Iinclude

SRCS=*.c
OBJS=*.o
COMPILE=./compile
MAKELIB=./makelib
SHAREDLIB=./sharedlib

default: clean check libs 

check:
	@[ -f $(COMPILE) ] && [ -f $(MAKELIB) ] || ./configure

clean:
	@echo -n Cleaning up libs ...
	@rm -f `cat TARGETS`
	@cd dnsstub ; make -s clean
	@echo " done!"

core: alloc.a buffer.a case.a cdb.a env.a logmsg.a fd.a fs.a getln.a getopt.a \
lock.a ndelay.a open.a seek.a sig.a str.a stralloc.a time.a wait.a \
ip.a socket.a dnsresolv.a 

libs: obj core qlibs.a dnsresolv.a

obj:
	@echo Making all in qlibs ...
	$(COMPILE) $(CCFLAGS) $(CFLAGS) $(SRCS)

install: setup

setup:
	@./install

shared: $(COMPILE) $(SHAREDLIB) $(OBJS) ./dnsstub/$(OBJS)
	@echo Building shared qlibs ...
	$(SHAREDLIB) $(LDFLAGS) $(OBJS) -o libqlibs.so 
	$(SHAREDLIB) $(LDFLAGS) ./dnsstub/$(OBJS) -o libdnsresolv.so
 
qlibs.a: obj socket.a 
	$(MAKELIB) $(LDFLAGS) qlibs.a *.o 
	ln -sf qlibs.a libqlibs.a

dnsresolv.a:
	@echo "Building @dnsresolv lib ..."
	@cd dnsstub ; make
	@cp dnsstub/dnsresolv.a dnsresolv.a
	ln -sf dnsresolv.a libdnsresolv.a

socket.a: ip.a
	@echo "Building socket lib ..."
	$(MAKELIB) $(LDFLAGS) socket.a socket_if.o socket_bind.o socket_connect.o \
	socket_info.o socket_setup.o socket_tcp.o socket_udp.o ip.a 

# build each file separately (backwards compat) - better use recipe qlibs.a
alloc.a: alloc.o
	$(MAKELIB) $(LDFLAGS) alloc.a alloc.o

buffer.a: buffer.o
	$(MAKELIB) $(LDFLAGS) buffer.a buffer.o 

case.a: case.o
	$(MAKELIB) $(LDFLAGS) case.a case.o

cdb.a: cdbread.o cdbmake.o uint32p.o seek.o buffer.o
	$(MAKELIB) $(LDFLAGS) cdb.a cdbread.o cdbmake.o uint32p.o seek.o buffer.o

env.a: env.o
	$(MAKELIB) $(LDFLAGS) env.a env.o

fd.a: fd.o
	$(MAKELIB) $(LDFLAGS) fd.a fd.o

fs.a: fmt.o scan.o
	$(MAKELIB) $(LDFLAGS) fs.a fmt.o scan.o

getln.a: getln.o
	$(MAKELIB) $(LDFLAGS) getln.a getln.o

getopt.a: getoptb.o
	$(MAKELIB) $(LDFLAGS) getopt.a getoptb.o

ip.a: ip4.o ip6.o socket_if.o
	$(MAKELIB) $(LDFLAGS) ip.a ip4.o ip6.o socket_if.o

lock.a: lock.o
	$(MAKELIB) $(LDFLAGS) lock.a lock.o

ndelay.a: ndelay.o
	$(MAKELIB) $(LDFLAGS) ndelay.a ndelay.o

open.a: open.o
	$(MAKELIB) $(LDFLAGS) open.a open.o

seek.a: seek.o
	$(MAKELIB) $(LDFLAGS) seek.a seek.o

sig.a: sig.o
	$(MAKELIB) $(LDFLAGS) sig.a sig.o

str.a: str.o byte.o stralloc.o alloc.o
	$(MAKELIB) $(LDFLAGS) str.a str.o byte.o stralloc.o alloc.o

stralloc.a: stralloc.o alloc.o
	$(MAKELIB) $(LDFLAGS) stralloc.a stralloc.o alloc.o

logmsg.a: errstr.o logmsg.o
	$(MAKELIB) $(LDFLAGS) logmsg.a errstr.o logmsg.o

time.a: iopause.o tai.o taia.o timeout.o timeoutconn.o
	$(MAKELIB) $(LDFLAGS) time.a iopause.o tai.o taia.o timeout.o timeoutconn.o

wait.a: wait.o
	$(MAKELIB) $(LDFLAGS) wait.a wait.o
