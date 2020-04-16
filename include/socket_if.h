#ifndef SOCKETIF_H
#define SOCKETIF_H

/*
 *  Revision 20181125, Erwin Hoffmann
 *  - switched to 'uint_t.h'
 *  - changed 'socket_tcp' --> 'socket_tcp4', 'socket_udp' --> 'socket_udp4'
 *  - added 'socket_ip6optionskill'
 *  - reordered and included backword compatible IPv6 calls
 *  - commented multicast socket declarations
 *  - enlarged usage for ipv4socket -> dual bind IPv4/IPv6
*/

#define __APPLE_USE_RFC_3542   /* MacOS Anycast support */

#include "uint_t.h"

/* IPv4 only */
extern int socket_connect4(int,const char [4],uint16);
extern int socket_bind4(int,const char [4],uint16);
extern int socket_bind4_reuse(int,const char [4],uint16);
extern int socket_accept4(int,char *,uint16 *);	/* not used here */
extern int socket_send4(int,const char *,unsigned int,const char [4],uint16);
extern int socket_local4(int,char *,uint16 *);
extern int socket_remote4(int,char *,uint16 *);

/* IPv6 only */
extern int socket_connect6(int s,const char [16],uint16,uint32);
extern int socket_bind6(int s,const char [16],uint16,uint32);
extern int socket_bind6_reuse(int s,const char [16],uint16,uint32);
extern int socket_send6(int,const char *,unsigned int,const char [16],uint16,uint32);
extern const char* socket_getifname(uint32);
extern uint32 socket_getifidx(const char *);
extern int socket_ip6optionskill(int);
extern int socket_ip6anycast(int);

/* Common IPv4 & IPv6 */
extern int socket_accept(int s,char [16],uint16 *,uint32 *);
extern int socket_bind(int s,const char [16],uint16,uint32);
extern int socket_bind_reuse(int s,const char [16],uint16,uint32);
extern int socket_connect(int s,const char [16],uint16,uint32);
extern int socket_connected(int);
extern int socket_listen(int,int);
extern int socket_local(int,char [16],uint16 *,uint32 *);
extern int socket_recv(int,char *,unsigned int,char [16],uint16 *,uint32 *);
extern int socket_remote(int,char [16],uint16 *,uint32 *);
extern int socket_send(int,const char *,unsigned int,const char [16],uint16,uint32);
extern void socket_tryreservein(int,int);
extern int socket_ipoptionskill(int);
extern int socket_dualstack(int);
extern int socket_nodualstack(int);

/* Backward compatibility */
#define socket_accept6 socket_accept
#define socket_local6 socket_local
#define socket_recv6 socket_recv 
#define socket_remote6 socket_remote

/* TCP */
extern int socket_tcp(void);
extern int socket_tcp4(void);
extern int socket_tcp6(void);
extern int socket_tcpnodelay(int);

/* UDP */
extern int socket_udp(void);
extern int socket_udp4(void);
extern int socket_udp6(void);

/*********** For future use ***********************************/
/* enable sending udp packets to the broadcast address */
// extern int socket_broadcast(int);
/* join a multicast group on the given interface */
// extern int socket_mcjoin4(int,char *,char *);
// extern int socket_mcjoin6(int,char *,int);
/* leave a multicast group on the given interface */
// extern int socket_mcleave4(int,char *);
// extern int socket_mcleave6(int,char *);
/* set multicast TTL/hop count for outgoing packets */
// extern int socket_mcttl4(int,char);
// extern int socket_mcttl6(int,char);
/* enable multicast loopback */
// extern int socket_mcloop4(int,char);
// extern int socket_mcloop6(int,char);
/**************************************************************/

/************* ipv4socket *************************************/
/* ipv4socket -> former noipv6                                */
/*                                                            */
/* ipv4socket = 0 -> IPv6 and dual-stack binding IPv6+IPv4    */
/*                                                            */
/* ipv4socket = 1 -> bind to IPv4                             */
/*              IP = IPv6-mapped-IPv4 address recognized      */
/*                                                            */
/* ipv4socket = 2 -> force binding to IPv4 only               */
/*                                                            */
/**************************************************************/
extern int ipv4socket; 

#endif
