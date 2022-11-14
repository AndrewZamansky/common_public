#ifndef __ALTERNATIVE_TYPES_H
#define __ALTERNATIVE_TYPES_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "project_config.h"

#include "sys/types.h"


#ifndef _TIMEVAL_DEFINED
#define _TIMEVAL_DEFINED
struct timeval {
  time_t      tv_sec;
  suseconds_t tv_usec;
};

/* BSD time macros used by RTEMS code */
#if defined (__rtems__) || defined (__CYGWIN__)

/* Convenience macros for operations on timevals.
   NOTE: `timercmp' does not work for >= or <=.  */
#define	timerisset(tvp)		((tvp)->tv_sec || (tvp)->tv_usec)
#define	timerclear(tvp)		((tvp)->tv_sec = (tvp)->tv_usec = 0)
#define	timercmp(a, b, CMP) 						      \
  (((a)->tv_sec == (b)->tv_sec) ? 					      \
   ((a)->tv_usec CMP (b)->tv_usec) : 					      \
   ((a)->tv_sec CMP (b)->tv_sec))
#define	timeradd(a, b, result)						      \
  do {									      \
    (result)->tv_sec = (a)->tv_sec + (b)->tv_sec;			      \
    (result)->tv_usec = (a)->tv_usec + (b)->tv_usec;			      \
    if ((result)->tv_usec >= 1000000)					      \
      {									      \
	++(result)->tv_sec;						      \
	(result)->tv_usec -= 1000000;					      \
      }									      \
  } while (0)
#define	timersub(a, b, result)						      \
  do {									      \
    (result)->tv_sec = (a)->tv_sec - (b)->tv_sec;			      \
    (result)->tv_usec = (a)->tv_usec - (b)->tv_usec;			      \
    if ((result)->tv_usec < 0) {					      \
      --(result)->tv_sec;						      \
      (result)->tv_usec += 1000000;					      \
    }									      \
  } while (0)
#endif /* defined (__rtems__) || defined (__CYGWIN__) */
#endif /* !_TIMEVAL_DEFINED */


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

#ifdef  __cplusplus
}
#endif


#endif /* __ALTERNATIVE_TYPES_H */
