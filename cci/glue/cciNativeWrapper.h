#ifndef cciNativeWrapper_h
#define cciNativeWrapper_h
/* :::BEGIN LICENSE BLOCK:::
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
 *  :::END LICENSE BLOCK::: */

//--------------------------------------------------------
// File         : cciNativeWrapper.h
// Date         : 13 janv. 2009
// Author       : David Marteau
//--------------------------------------------------------

  /*
   * This class is used as container wrapper for passing
   * native pointer as a container, the container class should declare
   * a getNative() method.
   */

  template<class T,class ContainerInterface>
  class native_Container : public ContainerInterface {

  public:
    native_Container( T* nativePtr )
    : mPtr(nativePtr)
    {
      CCI_PRECONDITION(mPtr,"null pointer");
    }

    T* mPtr;

    // cciISupports implementation
    CCI_IMETHOD QueryInterface(DM_REFIID aIID,void** aInstancePtr) {
      // QI not allowed
      return CCI_ERROR_NO_INTERFACE;
    }

    CCI_IMETHOD_(cci_refcnt) AddRef(void)  { CCI_NOTREACHED("Addref()  not allowed"); return 2; }
    CCI_IMETHOD_(cci_refcnt) Release(void) { CCI_NOTREACHED("Release() not allowed"); return 1; }

    CCI_IMETHOD_(T*) GetNative() { return  mPtr; }

    operator ContainerInterface* () { return this; }
  };

  #define CCI_NATIVE( container )    ((container)->GetNative())
  #define CCI_IF_NATIVE( container ) ((container)?(container)->GetNative():dm_null) 

#define DECL_NATIVE_WRAPPER( T, ContainerInterface)              \
  CCI_NAMESPACE_BEGIN                                            \
   inline native_Container<T,ContainerInterface>                 \
   native_Wrapper( T* nativePtr ) {                              \
    return native_Container<T,ContainerInterface>(nativePtr);    \
  }                                                              \
  CCI_NAMESPACE_END

#endif /* cciNativeWrapper_h */
