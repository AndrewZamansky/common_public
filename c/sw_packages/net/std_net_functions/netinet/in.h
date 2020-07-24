#ifndef __CUSTOM_IN_H
#define __CUSTOM_IN_H


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

struct in6_addr {
   unsigned char   s6_addr[16];   /* IPv6 address */
};

// _IN_PORT_T_DECLARED defined in some types.h of gcc
#if !defined(_IN_PORT_T_DECLARED)
	typedef uint32_t in_port_t;
#endif

struct sockaddr_in6 {
	sa_family_t     sin6_family;   /* AF_INET6 */
	in_port_t       sin6_port;     /* port number */
	uint32_t        sin6_flowinfo; /* IPv6 flow information */
	struct in6_addr sin6_addr;     /* IPv6 address */
	uint32_t        sin6_scope_id; /* Scope ID (new in 2.4) */
};


#endif
