#ifndef __CUSTOM_NETDB_H
#define __CUSTOM_NETDB_H

#include "sys/socket.h"

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

#define AI_PASSIVE  0

struct addrinfo {
   int              ai_flags;
   int              ai_family;
   int              ai_socktype;
   int              ai_protocol;
   socklen_t        ai_addrlen;
   struct sockaddr *ai_addr;
   char            *ai_canonname;
   struct addrinfo *ai_next;
};


int getaddrinfo(const char *node, const char *service,
					   const struct addrinfo *hints,
					   struct addrinfo **res);
void freeaddrinfo(struct addrinfo *res);

struct hostent*  gethostbyname( const char *name);

#endif
