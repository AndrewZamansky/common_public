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

#if defined(CONFIG_HOST)
	#include <sys/select.h>
#else
	#define R_OK  0
#endif

// some files compiled with win32/64 switch so if it's
// compiled with gcc that doesn't _SSIZE_T_DEFINED but
// ssize_t type defined in type.h (example: linux )
// then _SSIZE_T_DEFINED should be defined
#if defined(__ssize_t_defined) && !defined(_SSIZE_T_DEFINED)
	#define _SSIZE_T_DEFINED
#endif

# if !defined (FD_SETSIZE)
#  define _SYS_TYPES_FD_SET
#  define NBBY  8 /* number of bits in a byte */
/*
 * Select uses bit masks of file descriptors in longs.
 * These macros manipulate such bit fields (the filesystem macros use chars).
 * FD_SETSIZE may be defined by the user, but the default here
 * should be >= NOFILE (param.h).
 */
#  ifndef FD_SETSIZE
#    define  FD_SETSIZE 64
#  endif

typedef long  fd_mask;
#  define  NFDBITS (sizeof (fd_mask) * NBBY)  /* bits per mask */
#  ifndef  howmany
#   define howmany(x,y) (((x)+((y)-1))/(y))
#  endif

/* We use a macro for fd_set so that including Sockets.h afterwards
   can work.  */
typedef struct _types_fd_set {
	fd_mask  fds_bits[howmany(FD_SETSIZE, NFDBITS)];
} _types_fd_set;

#define fd_set _types_fd_set

#define FD_SET(n, p)  ((p)->fds_bits[(n)/NFDBITS] |= (1L << ((n) % NFDBITS)))
#define FD_CLR(n, p)  ((p)->fds_bits[(n)/NFDBITS] &= ~(1L << ((n) % NFDBITS)))
#define FD_ISSET(n, p)  ((p)->fds_bits[(n)/NFDBITS] & (1L << ((n) % NFDBITS)))
#define FD_ZERO(p) (__extension__ (void)({ \
		size_t __i; \
		char *__tmp = (char *)p; \
		for (__i = 0; __i < sizeof (*(p)); ++__i) \
		*__tmp++ = 0; \
}))

# endif


typedef uint32_t socklen_t ;

typedef struct in_addr {
union {
  struct {
	unsigned char s_b1,s_b2,s_b3,s_b4;
  } S_un_b;
  struct {
	unsigned short s_w1,s_w2;
  } S_un_w;
  unsigned long S_addr;
} S_un;
unsigned long s_addr;
}IN_ADDR_T;

struct sockaddr_in {
	  short   sin_family;
	  unsigned short sin_port;
	  struct  in_addr sin_addr;
	  char    sin_zero[8];
};

typedef struct sockaddr
{
  union {
	  struct {
		  unsigned short  sa_family;
		  char    sa_data[14];
	  };
	  struct sockaddr_in S_ub_sockaddr_in;
  };
}  SOCKADDR_T;



#ifdef CONFIG_USE_INTERNAL_SOCKETS_IMPLEMENTATION

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




enum {
  IPPROTO_IP = 0,               /* Dummy protocol for TCP               */
#define IPPROTO_IP              IPPROTO_IP
  IPPROTO_ICMP = 1,             /* Internet Control Message Protocol    */
#define IPPROTO_ICMP            IPPROTO_ICMP
  IPPROTO_IGMP = 2,             /* Internet Group Management Protocol   */
#define IPPROTO_IGMP            IPPROTO_IGMP
  IPPROTO_IPIP = 4,             /* IPIP tunnels (older KA9Q tunnels use 94) */
#define IPPROTO_IPIP            IPPROTO_IPIP
  IPPROTO_TCP = 6,              /* Transmission Control Protocol        */
#define IPPROTO_TCP             IPPROTO_TCP
  IPPROTO_EGP = 8,              /* Exterior Gateway Protocol            */
#define IPPROTO_EGP             IPPROTO_EGP
  IPPROTO_PUP = 12,             /* PUP protocol                         */
#define IPPROTO_PUP             IPPROTO_PUP
  IPPROTO_UDP = 17,             /* User Datagram Protocol               */
#define IPPROTO_UDP             IPPROTO_UDP
  IPPROTO_IDP = 22,             /* XNS IDP protocol                     */
#define IPPROTO_IDP             IPPROTO_IDP
  IPPROTO_TP = 29,              /* SO Transport Protocol Class 4        */
#define IPPROTO_TP              IPPROTO_TP
  IPPROTO_DCCP = 33,            /* Datagram Congestion Control Protocol */
#define IPPROTO_DCCP            IPPROTO_DCCP
  IPPROTO_IPV6 = 41,            /* IPv6-in-IPv4 tunnelling              */
#define IPPROTO_IPV6            IPPROTO_IPV6
  IPPROTO_RSVP = 46,            /* RSVP Protocol                        */
#define IPPROTO_RSVP            IPPROTO_RSVP
  IPPROTO_GRE = 47,             /* Cisco GRE tunnels (rfc 1701,1702)    */
#define IPPROTO_GRE             IPPROTO_GRE
  IPPROTO_ESP = 50,             /* Encapsulation Security Payload protocol */
#define IPPROTO_ESP             IPPROTO_ESP
  IPPROTO_AH = 51,              /* Authentication Header protocol       */
#define IPPROTO_AH              IPPROTO_AH
  IPPROTO_MTP = 92,             /* Multicast Transport Protocol         */
#define IPPROTO_MTP             IPPROTO_MTP
  IPPROTO_BEETPH = 94,          /* IP option pseudo header for BEET     */
#define IPPROTO_BEETPH          IPPROTO_BEETPH
  IPPROTO_ENCAP = 98,           /* Encapsulation Header                 */
#define IPPROTO_ENCAP           IPPROTO_ENCAP
  IPPROTO_PIM = 103,            /* Protocol Independent Multicast       */
#define IPPROTO_PIM             IPPROTO_PIM
  IPPROTO_COMP = 108,           /* Compression Header Protocol          */
#define IPPROTO_COMP            IPPROTO_COMP
  IPPROTO_SCTP = 132,           /* Stream Control Transport Protocol    */
#define IPPROTO_SCTP            IPPROTO_SCTP
  IPPROTO_UDPLITE = 136,        /* UDP-Lite (RFC 3828)                  */
#define IPPROTO_UDPLITE         IPPROTO_UDPLITE
  IPPROTO_MPLS = 137,           /* MPLS in IP (RFC 4023)                */
#define IPPROTO_MPLS            IPPROTO_MPLS
  IPPROTO_RAW = 255,            /* Raw IP packets                       */
#define IPPROTO_RAW             IPPROTO_RAW
  IPPROTO_MAX
};

typedef int sa_family_t;


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
struct in6_addr {
   unsigned char   s6_addr[16];   /* IPv6 address */
};

#if defined(CONFIG_LINUX_OS_WRAPPER)
	typedef uint32_t in_port_t;
#endif

struct sockaddr_in6 {
	sa_family_t     sin6_family;   /* AF_INET6 */
	in_port_t       sin6_port;     /* port number */
	uint32_t        sin6_flowinfo; /* IPv6 flow information */
	struct in6_addr sin6_addr;     /* IPv6 address */
	uint32_t        sin6_scope_id; /* Scope ID (new in 2.4) */
};


int WSAGetLastError(void);
void WSASetLastError(int iError);
int ioctlsocket(int socket, long   cmd, unsigned long *argp);


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