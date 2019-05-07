#ifndef dmConstruct_h
#define dmConstruct_h

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

/*
// new AND delete DECLARATIONS for in place construction/destruction operator
inline void* operator new(size_t, void *_P)  { return (_P); }
inline void  operator delete(void *, void *) { return;      }
*/
//------------------------------------------------------------------------
template<class T> inline T*   dmConstruct( T* p ) { return ::new(p) T(); }
template<class T> inline void dmDestroy  ( T* p ) { p->~T(); }

template<class T> inline T*   dmConstructPOD( T* p ) { return ::new(p) T; }
template<class T> inline void dmDestroyPOD  ( T* p ) { p->~T(); }


template<class T1, class T2>
inline	void dmConstruct( T1 *p, const T2& v) { ::new ((void  *)p) T1(v); }
//------------------------------------------------------------------------
#endif
