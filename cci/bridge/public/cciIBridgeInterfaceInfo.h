#ifndef cciIBridgeInterfaceInfo_h
#define cciIBridgeInterfaceInfo_h
/* :::BEGIN LICENSE BLOCK:::
 *
 * Copyright (c) 2004-2011 David Marteau
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
// File         : cciIBridgeInterfaceInfo.h
// Date         : 5 sept. 2011
// Author       : David Marteau
//--------------------------------------------------------

// Bridge interface descriptor

// {bc4f50e3-dc15-492a-aace-3dfb5d6d307d}
#define CCI_IBRIDGEINTERFACEINFO_IID \
  { 0xbc4f50e3, 0xdc15, 0x492a,  \
    { 0xaa, 0xce, 0x3d, 0xfb, 0x5d, 0x6d, 0x30, 0x7d } }
/**
 * ccIBridgeInterfaceInfo
 */
class CCI_NO_VTABLE cciIBridgeInterfaceInfo :  public cciISupports {
public:
    CCI_DECLARE_STATIC_IID_ACCESSOR(CCI_IBRIDGEINTERFACEINFO_IID);
    
    /**
     * Interface name
     */
    CCI_IMETHOD_(const char*) name() = 0;
    
    /**
     * Interface iid
     */
    CCI_IMETHOD_(const dmIID&) iid() = 0;
    
    /**
     * Return parent interface in case of inheritance
     */
    CCI_IMETHOD_(const cciIBridgeInterfaceInfo*) parent() = 0;
    
    /**
     * Interface tags
     */
    CCI_IMETHOD_(dm_uint) tags() = 0;
    
    /**
     * Return true if this interface is compatible
     * with a script 'function' declaration
     */
    CCI_IMETHOD_(bool) isFunction() = 0;
    
    /**
     * Return the number of methods
     * Note it include count for parents
     */
    CCI_IMETHOD_(dm_int) numMethods() = 0;

    /**
     * Return the number of constants
     * It include count for parents
     */
    CCI_IMETHOD_(dm_int) numConstants() = 0;

    /**
     * Return an ccIBridgeMethodInfo object that describe
     * the method from its index.
     * 
     * @param index the index of the method
     */
    CCI_IMETHOD_(const cciIBridgeMethodInfo*) getMethodByIndex( int index ) = 0;

    /**
     * Return an ccIBridgeMethodInfo object that describe
     * the method from its name.
     * 
     * @param name the name of the method
     */
    CCI_IMETHOD_(const cciIBridgeMethodInfo*) getMethod( const char* name ) = 0;
    
    /**
     * Helper for invoking a method from its index
     */
    CCI_IMETHOD invokeMethodByIndex( cciSupports* , int index, 
                                     cciBridge::returnValue&, 
                                     cciBridge::argumentStack& ) = 0;
    /**
     * Enumerate all methods
     */
    CCI_IMETHOD enumerateMethods( cciMethodEnumFunc enumCallback, void* data );

};

CCI_DEFINE_STATIC_IID_ACCESSOR(cciIBridgeMethod, CCI_IBRIDGEINTERFACEINFO_IID);



#endif /* cciIBridgeInterfaceInfo_h */
