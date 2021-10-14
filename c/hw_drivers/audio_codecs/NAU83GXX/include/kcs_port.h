
#ifndef _KCS_PORT_H_
#define _KCS_PORT_H_

#include "_project.h"

#define __DATATYPES_H

typedef signed   short int  Int16;    /**< signed 16bit integer number */

/**< signed fixed point integer Q0.15, one sign bit and 15 fractional bits */
typedef signed   short int  Frac16;
typedef unsigned       int  UInt32;    /**< unsigned 32bit integer number */

/**< signed fixed point integer Q0.31, one sign bit and 31 fractional bits */
typedef signed         int  Frac32;
typedef signed         int  Word32;   /**< signed 32bit integer number */

/**< signed fixed point integer Q1.62,
 *  one sign bit, one integer bit and 62 fractional bits */
typedef signed    long long Frac64;

#endif
