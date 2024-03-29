#ifndef __CUSTOM_SOCKET_H
#define __CUSTOM_SOCKET_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "project_config.h"
#include <stdint.h>
#include <sys/types.h>
#include "sys/time.h"
#include "errno.h"
#include "sys/types.h"

#include "alternative_types.h"

#ifdef CONFIG_USE_INTERNAL_SOCKETS_IMPLEMENTATION

typedef int sa_family_t;

#if defined(CONFIG_USE_INTERNAL_STD_NET_FUNCTION_IMPLEMENTATION)
	#include "netinet/in.h"// for struct sockaddr_in
#else
	struct sockaddr_in {uint8_t dummy};
#endif

typedef uint32_t socklen_t ;

typedef struct sockaddr
{
  union {
	  struct {
		  unsigned short  sa_family;
		  char    sa_data[14];
	  };
	  // put 'struct sockaddr_in' in union to reserve enough memory
	  struct sockaddr_in S_ub_sockaddr_in;
  };
}  SOCKADDR_T;



/*
* Types
*/
#define  SOCK_STREAM  1    /* stream socket */
#define  SOCK_DGRAM  2    /* datagram socket */
#define  SOCK_RAW  3    /* raw-protocol interface */
#define  SOCK_RDM  4    /* reliably-delivered message */
#define  SOCK_SEQPACKET  5    /* sequenced packet stream */

/*
* Option flags per-socket.
*/
#define  SO_DEBUG  0x0001    /* turn on debugging info recording */
#define  SO_ACCEPTCONN  0x0002    /* socket has had listen() */
#define  SO_REUSEADDR  0x0004    /* allow local address reuse */
#define  SO_KEEPALIVE  0x0008    /* keep connections alive */
#define  SO_DONTROUTE  0x0010    /* just use interface addresses */
#define  SO_BROADCAST  0x0020    /* permit sending of broadcast msgs */
#define  SO_USELOOPBACK  0x0040    /* bypass hardware when possible */
#define  SO_LINGER  0x0080    /* linger on close if data present */
#define  SO_OOBINLINE  0x0100    /* leave received OOB data in line */
#define  SO_BINDTODEVICE 0x0200

/*
* Additional options, not kept in so_options.
*/
#define SO_SNDBUF    1    /* send buffer size */
#define SO_RCVBUF    2    /* receive buffer size */
#define SO_SNDLOWAT  3    /* send low-water mark */
#define SO_RCVLOWAT  4    /* receive low-water mark */
#define SO_SNDTIMEO  5    /* send timeout */
#define SO_RCVTIMEO  6    /* receive timeout */
#define SO_ERROR     7    /* get error status and clear */
#define SO_TYPE      8    /* get socket type */
#define SO_NONBLOCK  9


#define TCP_NODELAY  0x1

/*
* Level number for (get/set)sockopt() to apply to socket itself.
*/
#define  SOL_SOCKET  0xffff    /* options for socket level */

/*
* Address families.
*/
#define  AF_UNSPEC  0    /* unspecified */
#define  AF_UNIX    1    /* local to host (pipes, portals) */
#define  AF_INET    2    /* internetwork: UDP, TCP, etc. */
#define  AF_IMPLINK  3    /* arpanet imp addresses */
#define  AF_PUP    4    /* pup protocols: e.g. BSP */
#define  AF_CHAOS  5    /* mit CHAOS protocols */
#define  AF_NS    6    /* XEROX NS protocols */
#define  AF_ISO    7    /* ISO protocols */
#define  AF_OSI    AF_ISO
#define  AF_ECMA    8    /* european computer manufacturers */
#define  AF_DATAKIT  9    /* datakit protocols */
#define  AF_CCITT  10    /* CCITT protocols, X.25 etc */
#define  AF_SNA    11    /* IBM SNA */
#define  AF_DECnet  12    /* DECnet */
#define  AF_DLI    13    /* DEC Direct data link interface */
#define  AF_LAT    14    /* LAT */
#define  AF_HYLINK  15    /* NSC Hyperchannel */
#define  AF_APPLETALK  16    /* Apple Talk */
#define  AF_ROUTE  17    /* Internal Routing Protocol */
#define  AF_LINK    18    /* Link layer interface */
#define  pseudo_AF_XTP  19    /* eXpress Transfer Protocol (no AF) */
#define  AF_IPC    20 /* inter-process communication */

#define  AF_MAX    20




// Desired design of maximum size and alignment.
#define _SS_MAXSIZE 128 // Implementation-defined maximum size

//Implementation-defined desired alignment
#define _SS_ALIGNSIZE (sizeof(int64_t))

// Definitions used for sockaddr_storage structure paddings design.
#define _SS_PAD1SIZE (_SS_ALIGNSIZE - sizeof(sa_family_t))
#define _SS_PAD2SIZE (_SS_MAXSIZE - (sizeof(sa_family_t)+ \
					  _SS_PAD1SIZE + _SS_ALIGNSIZE))

struct sockaddr_storage {
	sa_family_t  ss_family;  /* Address family. */
	/*
	 *  Following fields are implementation-defined.
	 */
	char _ss_pad1[_SS_PAD1SIZE];
	/* 6-byte pad; this is to make implementation-defined
	   pad up to alignment field that follows explicit in
	   the data structure. */
	int64_t _ss_align;  /* Field to force desired structure
						   storage alignment. */
	char _ss_pad2[_SS_PAD2SIZE];
	/* 112-byte pad to achieve desired size,
	   _SS_MAXSIZE value minus size of ss_family
	   __ss_pad1, __ss_align fields is 112. */
};




/* Bits in the FLAGS argument to `send', `recv', et al.  */
enum
{
  MSG_OOB                = 0x01,        /* Process out-of-band data.  */
#define MSG_OOB                MSG_OOB
  MSG_PEEK                = 0x02,        /* Peek at incoming messages.  */
#define MSG_PEEK        MSG_PEEK
  MSG_DONTROUTE        = 0x04,        /* Don't use local routing.  */
#define MSG_DONTROUTE        MSG_DONTROUTE
#ifdef __USE_GNU
  /* DECnet uses a different name.  */
  MSG_TRYHARD                = MSG_DONTROUTE,
# define MSG_TRYHARD        MSG_DONTROUTE
#endif
  MSG_CTRUNC                = 0x08,        /* Control data lost before delivery.  */
#define MSG_CTRUNC        MSG_CTRUNC
  MSG_PROXY                = 0x10,        /* Supply or ask second address.  */
#define MSG_PROXY        MSG_PROXY
  MSG_TRUNC                = 0x20,
#define MSG_TRUNC        MSG_TRUNC
  MSG_DONTWAIT        = 0x40, /* Nonblocking IO.  */
#define MSG_DONTWAIT        MSG_DONTWAIT
  MSG_EOR                = 0x80, /* End of record.  */
#define MSG_EOR                MSG_EOR
  MSG_WAITALL                = 0x100, /* Wait for a full request.  */
#define MSG_WAITALL        MSG_WAITALL
  MSG_FIN                = 0x200,
#define MSG_FIN                MSG_FIN
  MSG_SYN                = 0x400,
#define MSG_SYN                MSG_SYN
  MSG_CONFIRM                = 0x800, /* Confirm path validity.  */
#define MSG_CONFIRM        MSG_CONFIRM
  MSG_RST                = 0x1000,
#define MSG_RST                MSG_RST
  MSG_ERRQUEUE        = 0x2000, /* Fetch message from error queue.  */
#define MSG_ERRQUEUE        MSG_ERRQUEUE
  MSG_NOSIGNAL        = 0x4000, /* Do not generate SIGPIPE.  */
#define MSG_NOSIGNAL        MSG_NOSIGNAL
  MSG_MORE                = 0x8000,  /* Sender will send more.  */
#define MSG_MORE        MSG_MORE
  MSG_WAITFORONE        = 0x10000, /* Wait for at least one packet to return.*/
#define MSG_WAITFORONE        MSG_WAITFORONE
  MSG_BATCH                = 0x40000, /* sendmmsg: more messages coming.  */
#define MSG_BATCH        MSG_BATCH
  MSG_FASTOPEN        = 0x20000000, /* Send data in TCP SYN.  */
#define MSG_FASTOPEN        MSG_FASTOPEN
  MSG_CMSG_CLOEXEC        = 0x40000000        /* Set close_on_exit for file
                                         descriptor received through
                                         SCM_RIGHTS.  */
#define MSG_CMSG_CLOEXEC MSG_CMSG_CLOEXEC
};




extern uint16_t htons(uint16_t n);
uint32_t htonl(uint32_t n);
uint32_t ntohl(uint32_t n);

#if defined(CONFIG_USE_INTERNAL_SOCKETS_TOGETHER_WITH_SYSTEM) && \
	!defined(TEMPORARY_STOP_SOCKET_RENAIMING)
	#define socket  socket_uCprojects
	#define send  send_uCprojects
	#define recv  recv_uCprojects
	#define getpeername  getpeername_uCprojects
	#define getsockname  getsockname_uCprojects
	#define getsockopt  getsockopt_uCprojects
	#define setsockopt  setsockopt_uCprojects
	#define connect  connect_uCprojects
	#define select  select_uCprojects
	#define gethostbyname  gethostbyname_uCprojects
	#define bind  bind_uCprojects
	#define accept  accept_uCprojects
	#define listen  listen_uCprojects
	#define shutdown  shutdown_uCprojects
	#define closesocket  closesocket_uCprojects
#endif

ssize_t send(
		int socket, const void *buffer, size_t length, int flags);
ssize_t recv(int sockfd, void *buf, size_t len, int flags);
int select(int nfds, fd_set *readfds, fd_set *writefds,
                  fd_set *exceptfds, struct timeval *timeout);
int getpeername(
		int sockfd, struct sockaddr *addr, socklen_t *addrlen);
int getsockname(
		int sockfd, struct sockaddr *local_addr, socklen_t *addrlen);
int getsockopt(int sockfd, int level, int optname,
			void *optval, socklen_t *optlen);
int setsockopt(int sockfd, int level,
		int optname, const void *optval, socklen_t optlen);
int connect(
		int sockfd, const struct sockaddr *addr, unsigned int addrlen);
int socket(int socket_family, int socket_type, int protocol);
uint16_t ntohs(uint16_t n);
struct servent *getservbyname(const char *name, const char *proto);
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int listen(int sockfd, int backlog);
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
int shutdown(int socket, int how);
int closesocket(int sockfd);
ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
                 struct sockaddr *src_addr, socklen_t *addrlen);

#endif /* CONFIG_USE_INTERNAL_SOCKETS_IMPLEMENTATION */

#ifdef  __cplusplus
}
#endif


#endif /* __CUSTOM_SOCKET_H */
