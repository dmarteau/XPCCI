#ifndef cciBridgeSupports_h
#define cciBridgeSupports_h
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
// File         : cciBridgeSupports.h
// Date         : 2 sept. 2011
// Author       : David Marteau
//--------------------------------------------------------

#include "cciSupports.h"

namespace cciBridge
{

class paramType 
{
public:
  paramType( dm_uint8 _flags, dm_uint8 _argnum = 0, dm_uint8 _argnum2 = 0, dm_uint16 _additional_type = 0)
  : flags(_flags)
  , argnum(_argnum)
  , argnum2(_argnum2)
  {
    type.additional_type = _additional_type;
  }

public:
  bool isPointer()    const 
    { return (flags & T_POINTER)!=0; }
  bool isReference()  const 
    { return (flags & T_REFERENCE)!=0; }
  bool isArithmetic() const 
    { return (flags <= T_WCHAR); }
  bool isInterfacePointer() const
    { switch(tag()) {
        default:  return false;
        case T_INTERFACE:
        case T_INTERFACE_IS:
          return true;
      }
    }
  bool isArray() const 
    { return tag() == T_ARRAY; }
  dm_uint8 tag() const
    { return (flags & T_TAGMASK); }
        
public:     
  enum {
    T_POINTER    = 0x80,
    T_REFERENCE  = 0x20,
    T_FLAGMASK   = 0xe0,
    T_TAGMASK    = (~T_FLAGMASK)
  };
  
  enum {
    T_INT8              = 0,
    T_INT16             = 1,
    T_INT32             = 2,
    T_INT64             = 3,
    T_UINT8             = 4,
    T_UINT16            = 5,
    T_UINT32            = 6,
    T_UINT64            = 7,
    T_FLOAT             = 8, 
    T_DOUBLE            = 9,
    T_BOOL              = 10,  
    T_CHAR              = 11,  
    T_WCHAR             = 12, 
    T_VOID              = 13,  
    T_IID               = 14,
    T_DOMSTRING         = 15,
    T_PSTRING           = 16,
    T_PWSTRING          = 17,
    T_INTERFACE         = 18,
    T_INTERFACE_IS      = 19,
    T_ARRAY             = 20,
    T_PSTRING_SIZE_IS   = 21,
    T_PWSTRING_SIZE_IS  = 22,
    T_UTF8STRING        = 23,
    T_CSTRING           = 24,
    T_ASTRING           = 25    
  };
  
protected:
  dm_uint8 flags;
  dm_uint8 argnum;    // used for iid_is and size_is
  dm_uint8 argnum2;   // used for length_is
  union {                         
    dm_uint16 iface;             // used for T_INTERFACE
    dm_uint16 additional_type;   // used for T_ARRAY
  } type;
};

}


#endif /* cciBridgeSupports_h */
