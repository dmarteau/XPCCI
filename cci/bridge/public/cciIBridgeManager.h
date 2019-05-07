#ifndef cciIBridgeManager_h
#define cciIBridgeManager_h
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
// File         : cciIBridgeManager.h
// Date         : 2 sept. 2011
// Author       : David Marteau
//--------------------------------------------------------

#ifndef __gen_cciISupports_h__
#include "cciISupports.h"
#endif

#include "cciBridgeSupports.h"

class cciIBridgeInterface;
class cciIBridgeMethod;

typedef bool (* cciMethodEnumFunc)(const cciIBridgeMethod*, void *aData);

// {b309dbe5-3f59-44f0-9b3b-a7fd63858c93}
#define CCI_BRIDGEMANAGER_CID \
  { 0xb309dbe5, 0x3f59, 0x44f0, \
    { 0x9b, 0x3b, 0xa7, 0xfd, 0x63, 0x85, 0x8c, 0x93 } }

// {5e639722-2624-4256-9820-53c354ddf11a}
#define CCI_IBRIDGEMANAGER_IID \
  { 0x5e639722, 0x2624, 0x4256,  \
    { 0x98, 0x20, 0x53, 0xc3, 0x54, 0xdd, 0xf1, 0x1a } }

#define CCI_BRIDGEMANAGER_CONTRACTID "@daim.org/bridge-manager;1"
#define CCI_BRIDGEMANAGER_CLASSNAME  "Bridge Manager"

/**
 * cciIBridgeManager
 */
class CCI_NO_VTABLE cciIBridgeManager :  public cciISupports {
public:
    CCI_DECLARE_STATIC_IID_ACCESSOR(CCI_IBRIDGEMANAGER_IID);
    
    /**
     * Return an cciIBridgeInterfaceInfo object that describe
     * the interface given by its IID
     * 
     * @param aIID the IID of the requesr interface info
     */
    CCI_IMETHOD_(const cciIBridgeInterface*) getBridgeInterface( const DM_REFIID& aIID ) = 0;
};

CCI_DEFINE_STATIC_IID_ACCESSOR(cciIBridgeManager, CCI_IBRIDGEMANAGER_IID);



// Bridge method descriptor

// {44e4b070-9107-4068-b526-cba603225d6a}
#define CCI_IBRIDGEMETHOD_IID \
  { 0x44e4b070, 0x9107, 0x4068,  \
    { 0xb5, 0x26, 0xcb, 0xa6, 0x03, 0x22, 0x5d, 0x6a } }
/**
 * cciIBridgeMethod
 */
class CCI_NO_VTABLE cciIBridgeMethod :  public cciISupports {
public:
    CCI_DECLARE_STATIC_IID_ACCESSOR(CCI_IBRIDGEMETHOD_IID);

    /**
     * Return the methode name
     */
    CCI_IMETHOD_(const char*) name() = 0;
    
    /**
     * Index of the method
     */
    CCI_IMETHOD_(dm_int) index() = 0;
    
    /**
     * Invoke the method
     */
    CCI_IMETHOD invoke( cciSupports*,
                        cciBridge::returnValue&, 
                        cciBridge::argumentStack& ) = 0;
   
};


CCI_DEFINE_STATIC_IID_ACCESSOR(cciIBridgeMethod, CCI_IBRIDGEMETHOD_IID);

#endif /* cciIBridgeManager_h */
