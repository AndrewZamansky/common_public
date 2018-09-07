#ifndef __CUSTOM_SOCKET_H
#define __CUSTOM_SOCKET_H

#ifdef  __cplusplus
extern "C" {
#endif


#include <stdint.h>
#include <sys/types.h>
#include <sys/select.h>



typedef uint32_t socklen_t ;

typedef struct sockaddr
{
  unsigned short  sa_family;
  char    sa_data[14];
}  SOCKADDR_T;

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

typedef struct hostent {
char       *h_name;
char    **h_aliases;
short         h_addrtype;
short         h_length;
char    **h_addr_list;
}HOSTENT_T;

struct servent {
//    char  *s_name;       /* official service name */
//    char **s_aliases;    /* alias list */
    int    s_port;       /* port number */
    char  *s_proto;      /* protocol to use */
};

#ifdef USE_CUSTOM_SOCKET_IN_COMPILED_MODULE

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


int WSAGetLastError(void);
void WSASetLastError(int iError);
int ioctlsocket(int socket, long   cmd, unsigned long *argp);


extern uint16_t htons(uint16_t n);
uint32_t htonl(uint32_t n);
uint32_t ntohl(uint32_t n);


ssize_t send_uCprojects(
		int socket, const void *buffer, size_t length, int flags);
size_t recv_uCprojects(int sockfd, void *buf, size_t len, int flags);
int select_uCprojects(int nfds, fd_set *readfds, fd_set *writefds,
                  fd_set *exceptfds, struct timeval *timeout);
struct hostent*  gethostbyname_uCprojects( const char *name);
int getpeername_uCprojects(
		int sockfd, struct sockaddr *addr, socklen_t *addrlen);
int getsockname_uCprojects(
		int sockfd, struct sockaddr *local_addr, socklen_t *addrlen);
int getsockopt_uCprojects(int sockfd, int level, int optname,
			void *optval, socklen_t *optlen);
int setsockopt_uCprojects(int sockfd, int level,
		int optname, const void *optval, socklen_t optlen);
int connect_uCprojects(
		int sockfd, const struct sockaddr *addr, unsigned int addrlen);
int socket_uCprojects(int socket_family, int socket_type, int protocol);
uint16_t ntohs(uint16_t n);
struct servent *getservbyname(const char *name, const char *proto);
int bind_uCprojects(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int listen_uCprojects(int sockfd, int backlog);
int accept_uCprojects(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
int shutdown_uCprojects(int socket, int how);
int closesocket_uCprojects(int sockfd);

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


#endif /* USE_CUSTOM_SOCKET_IN_COMPILED_MODULE */

#ifdef  __cplusplus
}
#endif


#endif /* __CUSTOM_SOCKET_H */
