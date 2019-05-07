#ifndef dmTypes_h
#define dmTypes_h

/* ::: BEGIN LICENSE BLOCK:::
 *
 * Copyright (c) 2004-2005 David Marteau
 *
 * This file is part of the DAIM Image Processing Library (DAIM library)
 *
 * The DAIM library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License (LGPL)
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * The DAIM library is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with the DAIM library; see the file LICENSE.  If
 * not, write to the Free Software Foundation, Inc., 59 Temple Place -
 * Suite 330, Boston, MA 02111-1307, USA.
 *
 *  ::: END LICENSE BLOCK::: */

//--------------------------------------------------------
// File         : dmTypes.h
// Date         : 4/2008
// updated      : 03/2012 
//                Use header file dmConfig.h
// Author       : David Marteau
//--------------------------------------------------------

#if defined(DM_CONFIG_WIN32_WIN32) && !defined(DM_CONFIG_STRIP_HEADER)
  #include <wtypes.h> // types definition for W32
#else
  #include <stddef.h>
#endif

// Not including this at the very beginning
// may lead to compilation errors with OSX.
#include <math.h>

#include "ac_dm_config.h"

// Set the assumed value for sizeof(int)
#ifndef DM_BYTES_PER_INT
#define DM_BYTES_PER_INT DM_ASSUME_BYTES_PER_INT
#endif

/**
 * 8 bits signed/unsigned quantities
 */
typedef char            dm_int8;    // signed 8-bit quantity
typedef unsigned char   dm_uint8;   // unsigned 8-bit quantity

#define DM_INT8_MAX 127
#define DM_INT8_MIN (-128)
#define DM_UINT8_MAX 255U

/**
 * 16 bits signed/unsigned
 */

typedef short           dm_int16;   // signed 16-bit quantity
typedef unsigned short  dm_uint16;  // unsigned 16-bit quantity

#define DM_INT16_MAX 32767
#define DM_INT16_MIN (-32768)
#define DM_UINT16_MAX 65535U

/**
 * 32 bits signed/unsigned quantities
 */

#if DM_BYTES_PER_INT >= 4

typedef unsigned int dm_uint32;   // signed 32-bit quantity
typedef int          dm_int32;    // unsigned 32-bit quantity

#define DM_INT32(x)  x
#define DM_UINT32(x) x ## U

#elif DM_BYTES_PER_LONG >= 4

typedef unsigned long dm_uint32;
typedef long          dm_int32;

#define DM_INT32(x)  x ## L
#define DM_UINT32(x) x ## UL

#endif

#define DM_INT32_MAX DM_INT32(2147483647)
#define DM_INT32_MIN (-DM_INT32_MAX - 1)
#define DM_UINT32_MAX DM_UINT32(4294967295)

/**
 * 64 bits signed/unsigned quantities
 */

#ifdef DM_HAVE_LONG_LONG

#if DM_BYTES_PER_LONG >= 8

typedef long          dm_int64;
typedef unsigned long dm_uint64;

#define DM_INT64(x)  x ## L
#define DM_UINT64(x) x ## UL

#elif defined(DM_CONFIG_WIN32_WIN32) && !defined(__GNUC__)

typedef          __int64 dm_int64;
typedef unsigned __int64 dm_uint64;

#define DM_INT64(x)  x ## i64
#define DM_UINT64(x) x ## ui64

#else

typedef long long          dm_int64;
typedef unsigned long long dm_uint64;

#define DM_INT64(x)  x ## LL
#define DM_UINT64(x) x ## ULL

#endif // DM_BYTES_PER_LONG >= 8

#define DM_INT64_MAX DM_INT64(0x7fffffffffffffff)
#define DM_INT64_MIN (-DM_INT64_MAX - 1)
#define DM_UINT64_MAX DM_UINT64(-1)

#else  // !DM_HAVE_LONG_LONG

typedef struct {
#ifdef DM_IS_LITTLE_ENDIAN
    dm_uint32 lo, hi;
#else
    dm_uint32 hi, lo;
#endif
} dm_int64;
typedef dm_int64 dm_uint64;

#define DM_INT64_MAX (dm_int64){0x7fffffff, 0xffffffff}
#define DM_INT64_MIN (dm_int64){0xffffffff, 0xffffffff}
#define DM_UINT64_MAX (dm_uint64){0xffffffff, 0xffffffff}

#endif /* !DM_HAVE_LONG_LONG */

//XXX Remove dm_int references
#if DM_BYTES_PER_INT >= 2
// dm_int/dm_uint is semantically the type use for integer calculation
// Choose here the most suitable  (or fastest) representation for an integer
// (size is not an issue for that type)
typedef long          dm_int;
typedef unsigned long dm_uint;
#else
#error 'sizeof(int)' not sufficient for platform use
#endif


/**
 *  A type for representing byte offsets from some location.
 */
typedef dm_int32 dm_offset32;
typedef dm_int64 dm_offset64;

/**
 * generic pointer
 */

typedef void*  dm_ptr;


/**
 * unicode
 */

typedef wchar_t dm_unichar;

/**
 * helper types
 */

#define dm_byte  dm_uint8
#define dm_size  size_t

typedef double       dm_real;

// define param type as a long (dm_long_ptr) value
#define dm_param_t( x )  dm_param(x)

// We need to define this when using dlopen
#if defined(DM_CONFIG_TARGET_UNIX) || defined(DM_CONFIG_TARGET_CYGWIN)

  #define dm_hmodule void*

#elif defined(DM_CONFIG_WIN32_WIN32)

  #define dm_hmodule  HMODULE // win32 module handle

#endif

// Define thread id type
#define dm_thread_t unsigned long

/*
 * These macros allow you to give a hint to the compiler about branch
 * probability so that it can better optimize.  Use them like this:
 *
 *  if (DM_LIKELY(v == 1)) {
 *    ... expected code path ...
 *  }
 *
 *  if (DM_UNLIKELY(v == 0)) {
 *    ... non-expected code path ...
 *  }
 *
 * These macros are guaranteed to always return 0 or 1.
 * The CCI_FAILED/CCI_SUCCEEDED macros depends on this.
 * @return 0 or 1
 */

#if defined(__GNUC__) && (__GNUC__ > 2)
#define DM_LIKELY(x)    (__builtin_expect(!!(x), 1))
#define DM_UNLIKELY(x)  (__builtin_expect(!!(x), 0))
#else
#define DM_LIKELY(x)    (!!(x))
#define DM_UNLIKELY(x)  (!!(x))
#endif

/*
 *  Macro body brackets so that macros with compound statement definitions
 *  behave syntactically more like functions when called.
 */
#define DM_BEGIN_MACRO  do {
#define DM_END_MACRO    } while (0)

#define dm_null     0
#define dm_noindex DM_UINT32(~0)

// need to define a generic return callback function
// this is needed when compiler (such as gcc) enforce
// ISO C++ (where casting between pointer-to-object and
// pointer-to-function is forbidden

typedef int (*dm_func_ptr)();

#define dm_mask8   0xFF
#define dm_mask16  0xFFFF
#define dm_mask32  0xFFFFFFFF

#define dm_array( t, n ) t*

#ifdef __cplusplus

#define CONST const

template<class T> inline T   dm_max( const T& a, const T& b ) { return (a>b ? a:b); }
template<class T> inline T   dm_min( const T& a, const T& b ) { return (a<b ? a:b); }
template<class T> inline T   dm_abs( const T& a ) { return (a<0 ?-a:a); }
template<class T> inline int dm_sgn( const T& a ) { return (a<0?-1:1); }

#else // __cplusplus

#define CONST

#define dm_abs(a)   ( ((a)<0)   ? -(a) : (a))
#define dm_max(a,b) ( ((a)>(b)) ? (a):(b) )
#define dm_min(a,b) ( ((a)<(b)) ? (a):(b) )
#define dm_sgn(a)   ( ((a)<0)   ? -1 : 1  )

#endif // __cpluplus

#define dm_make_id(a,b,c,d)   \
       ((dm_uint32) (a)<<24 | \
        (dm_uint32) (b)<<16 | \
        (dm_uint32) (c)<<8  | \
        (dm_uint32) (d))

#define dm_align32( n ) ( ( ((n)+3) >> 2 ) * 4 )
#define dm_roundup(x,y) ((((x)+((y)-1))/(y))*(y))

#define DM_ARRAY_LENGTH(array_) \
  (sizeof(array_)/sizeof(array_[0]))

#define DM_UNUSUED( unusued_content_ ) //

// Max path is not defined set default to 256
#ifndef MAX_PATH
  #define MAX_PATH 256
#endif

// Used a level for log messages
enum {
  DM_LOG_MESSAGE   = 0,
  DM_LOG_WARNING   = 1,
  DM_LOG_ERROR     = 2
};

// Debug assertion level
enum {
    DM_DEBUG_WARNING   = 0,
    DM_DEBUG_ASSERTION = 1,
    DM_DEBUG_BREAK     = 2,
    DM_DEBUG_ABORT     = 3
};


#endif /* dmTypes_h */

