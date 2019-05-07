#ifndef dmCrt_h
#define dmCrt_h

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
#ifndef DM_NO_STD_INCLUDES
  #include <stdlib.h>  // for realloc()
  #include <string.h>  // for memmove()
#endif

#define DM_LL(s) L##s

#ifndef DM_NO_CRT_STRING_API

#include <stdarg.h> // for va_list
#include <stdio.h>  // for vsnprintf


// Map visual fonction name to more POSIX compliant
// WARNING: return values may differ varying to implementation
#if defined(DM_MSVCRT_BUILD)

  #define strcasecmp _stricmp
  #define strncasecmp _strnicmp
  #define wcscasecmp _wcsicmp
  #define wcsncasecmp _wcsnicmp
  #define vswprintf _vsnwprintf
  #define snprintf _snprintf

#elif defined(DM_CONFIG_POSIX_OK)

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <ctype.h>
#include <wctype.h>
#include <wchar.h>


#ifdef DM_NO_GNU_EXTENSION
#include "dmCrt.cpp"
#endif


#endif // DM_MSVCRT_BUILD


// Ascii
inline size_t _dm_strlen(const char* str) { return strlen(str); }
inline int    _dm_toupper(char c)         { return toupper(c);  }
inline int    _dm_tolower(char c)         { return tolower(c);  }
inline int    _dm_strcmp(const char* s1,const char* s2)                 { return strcmp(s1,s2); }
inline int    _dm_strcasecmp(const char* s1,const char* s2 )            { return strcasecmp(s1,s2); }
inline int    _dm_strncmp(const char* s1,const char* s2, size_t n)      { return strncmp(s1,s2,n); }
inline int    _dm_strncasecmp(const char* s1,const char* s2, size_t n)  { return strncasecmp(s1,s2,n); }
inline int    _dm_strtol(const char *nptr, char **endptr, int base)     { return strtol(nptr,endptr,base); }
inline int    _dm_strtoul(const char *nptr, char **endptr, int base)    { return strtoul(nptr,endptr,base); }
inline int    _dm_vsnprintf(char *str, size_t size, const char *format, va_list ap) {
     return  vsnprintf(str,size,format,ap);
}


#ifndef DM_CONFIG_NO_CRT_STRING_API_OVERLOAD

// Unicode
inline size_t _dm_strlen(const wchar_t* str) { return wcslen(str); }
inline int    _dm_toupper(wchar_t c)         { return towupper(c); }
inline int    _dm_tolower(wchar_t c)         { return towlower(c); }
inline int    _dm_strcmp(const wchar_t* s1,const wchar_t* s2)                 { return wcscmp(s1,s2); }
inline int    _dm_strncmp(const wchar_t* s1,const wchar_t* s2, size_t n)      { return wcsncmp(s1,s2,n); }
inline int    _dm_strncasecmp(const wchar_t* s1,const wchar_t* s2, size_t n)  { return wcsncasecmp(s1,s2,n); }
inline int    _dm_strcasecmp(const wchar_t* s1,const wchar_t* s2 )            { return wcscasecmp(s1,s2); }
inline int    _dm_strtol(const wchar_t *nptr, wchar_t **endptr, int base)     { return wcstol(nptr,endptr,base); }
inline int    _dm_strtoul(const wchar_t *nptr, wchar_t **endptr, int base)    { return wcstoul(nptr,endptr,base); }
inline int    _dm_vsnprintf(wchar_t *str, size_t size, const wchar_t *format, va_list ap) {
     return  vswprintf(str,size,format,ap);
}

#endif // DM_CONFIG_NO_CRT_STRING_API_OVERLOAD

// Prefixed names
inline size_t _dm_wstrlen(const wchar_t* str) { return wcslen(str); }
inline int    _dm_wtoupper(wchar_t c)         { return towupper(c); }
inline int    _dm_wtolower(wchar_t c)         { return towlower(c); }
inline int    _dm_wstrcmp(const wchar_t* s1,const wchar_t* s2)                  { return wcscmp(s1,s2); }
inline int    _dm_wstrncmp(const wchar_t* s1,const wchar_t* s2, size_t n)       { return wcsncmp(s1,s2,n); }
inline int    _dm_wstrncasecmp(const wchar_t* s1,const wchar_t* s2, size_t n)   { return wcsncasecmp(s1,s2,n); }
inline int    _dm_wstrcasecmp(const wchar_t* s1,const wchar_t* s2 )             { return wcscasecmp(s1,s2); }
inline int    _dm_wstrtol(const wchar_t *nptr, wchar_t **endptr, int base)      { return wcstol(nptr,endptr,base); }
inline int    _dm_wstrtoul(const wchar_t *nptr, wchar_t **endptr, int base)     { return wcstoul(nptr,endptr,base); }
inline int    _dm_wvsnprintf(wchar_t *str, size_t size, const wchar_t *format, va_list ap) {
     return  vswprintf(str,size,format,ap);
}

#endif // DM_NO_CRT_STRING_API

#endif // dmCrt_h

