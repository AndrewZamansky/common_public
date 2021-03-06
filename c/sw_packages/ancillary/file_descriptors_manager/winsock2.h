/*dummy ws2tcpip.h*/
#ifndef _WS2TCPIP_H_
#define _WS2TCPIP_H_

#include "sys/socket.h"

#define SOCKET_ERROR 1
#define WSAEMSGSIZE  2
#define FIONBIO  0

#define MAKEWORD(bLow, bHigh) ((bHigh << 8) & bLow)
typedef int32_t WORD;

#define WSADESCRIPTION_LEN 2
#define WSASYS_STATUS_LEN  2

#define WSAEWOULDBLOCK  EAGAIN
#define WSAEAGAIN  EAGAIN
#define WSAECONNRESET ECONNRESET
#define WSAEINTR  EINTR
#define WSAECONNABORTED  ECONNABORTED
#define WSAETIMEDOUT  ETIMEDOUT

typedef struct WSAData {
        WORD                    wVersion;
        WORD                    wHighVersion;
        char                    szDescription[WSADESCRIPTION_LEN+1];
        char                    szSystemStatus[WSASYS_STATUS_LEN+1];
        unsigned short          iMaxSockets;
        unsigned short          iMaxUdpDg;
        char  *              lpVendorInfo;
} WSADATA, * LPWSADATA;

int WSAStartup(WORD wVersionRequired, LPWSADATA lpWSAData);
void Sleep(int a);
int WSAGetLastError(void);
void WSASetLastError(int iError);
int ioctlsocket(int socket, long   cmd, unsigned long *argp);

#endif //_WS2TCPIP_H_
