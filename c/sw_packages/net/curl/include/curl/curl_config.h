#ifndef HEADER_CURL_CONFIG_H
#define HEADER_CURL_CONFIG_H

#define HAVE_ERRNO_H 1

#define HAVE_SYS_STAT_H 1

#define HAVE_SELECT 1

#define HAVE_LONGLONG

#define HAVE_ASSERT_H	1

#define HAVE_STRUCT_TIMEVAL 1
#define HAVE_SYS_TIME_H 1
#define HAVE_SYS_SOCKET_H 1

#define OS "freeRTOS"

#define USE_BLOCKING_SOCKETS
/* The size of `int', as computed by sizeof. */
#define SIZEOF_INT 4

/* The size of `off_t', as computed by sizeof. */
#define SIZEOF_OFF_T 8

/* The size of `short', as computed by sizeof. */
#define SIZEOF_SHORT 2

/* The size of `size_t', as computed by sizeof. */
#define SIZEOF_SIZE_T 4

/* The size of `time_t', as computed by sizeof. */
#define SIZEOF_TIME_T 4

/* The size of `void*', as computed by sizeof. */
#define SIZEOF_VOIDP 4


/* Define to 1 if you have the send function. */
#define HAVE_SEND 1

/* Define to the type qualifier of arg 2 for send. */
#define SEND_QUAL_ARG2 const

/* Define to the type of arg 1 for send. */
#define SEND_TYPE_ARG1 int

/* Define to the type of arg 2 for send. */
#define SEND_TYPE_ARG2 void *

/* Define to the type of arg 3 for send. */
#define SEND_TYPE_ARG3 size_t

/* Define to the type of arg 4 for send. */
#define SEND_TYPE_ARG4 int

/* Define to the function return type for send. */
#define SEND_TYPE_RETV int


/* Define to 1 if you have the recv function. */
#define HAVE_RECV 1

/* Define to the type of arg 1 for recv. */
#define RECV_TYPE_ARG1 int

/* Define to the type of arg 2 for recv. */
#define RECV_TYPE_ARG2 void *

/* Define to the type of arg 3 for recv. */
#define RECV_TYPE_ARG3 size_t

/* Define to the type of arg 4 for recv. */
#define RECV_TYPE_ARG4 int

/* Define to the function return type for recv. */
#define RECV_TYPE_RETV int

#define HAVE_SOCKET	1

#endif /* HEADER_CURL_CONFIG_VXWORKS_H */
