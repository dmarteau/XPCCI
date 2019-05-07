
/* ::: BEGIN LICENSE BLOCK:::
 *
 * Copyright (c) 2008 David Marteau
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

#include "cciString.h"
#include "cciDebug.h"

extern "C" {


__daim_export cci_result
CCI_CStringContainerInit( cciCStringContainer &aContainer )
{
  CCI_ASSERTION(sizeof(cciStringContainer_base) >= sizeof(dmCString),
       "cciStringContainer is not large enough");

  // use placement new to avoid heap allocating dmString object
  new (&aContainer) dmCString();

  return CCI_OK;
}

__daim_export void
CCI_CStringContainerFinish( cciCStringContainer &aContainer )
{
  // call the dmCString dtor
  reinterpret_cast<dmCString *>(&aContainer)->~dmCString();
}

__daim_export cci_result
CCI_CStringSetData( dmACString &aStr, const char  *aData, dm_uint32 aDataLength )
{
  aStr.Assign(aData,aDataLength);
  return CCI_OK;
}

__daim_export dm_uint32
CCI_CStringGetData( const dmACString &aStr, const char **aData )
{
  if(aData)
    *aData = aStr.get();

  return aStr.Size();
}


__daim_export cci_result
CCI_CStringAppendData( dmACString &aStr, const char  *aData, dm_uint32 aDataLength )
{
  if(aData)
  {
    if(aDataLength == dmACString::npos)
       aStr.Append(aData);
    else
       aStr.Append(aData,aDataLength);
  }
  return aStr.Size();
}

__daim_export cci_result
CCI_CStringCopy( dmACString &aDestStr, const dmACString& aSrcStr )
{
  aDestStr.Assign(aSrcStr);
  return CCI_OK;
}

// Unicode


__daim_export cci_result
CCI_StringContainerInit( cciStringContainer &aContainer )
{
  CCI_ASSERTION(sizeof(cciStringContainer_base) >= sizeof(dmString),
       "cciStringContainer is not large enough");

  // use placement new to avoid heap allocating dmString object
  new (&aContainer) dmString();

  return CCI_OK;
}

__daim_export void
CCI_StringContainerFinish( cciStringContainer &aContainer )
{
  // call the dmCString dtor
  reinterpret_cast<dmString *>(&aContainer)->~dmString();
}


__daim_export cci_result
CCI_StringSetData( dmAString &aStr, const dm_unichar  *aData, dm_uint32 aDataLength)
{
  aStr.Assign(aData,aDataLength);
  return CCI_OK;

}

__daim_export dm_uint32
CCI_StringGetData( const dmAString &aStr, const dm_unichar **aData )
{
  if(aData)
    *aData = aStr.get();

  return aStr.Size();
}


__daim_export cci_result
CCI_StringAppendData( dmAString &aStr, const dm_unichar *aData, dm_uint32 aDataLength )
{
  if(aData)
  {
    if(aDataLength == dmAString::npos)
       aStr.Append(aData);
    else
       aStr.Append(aData,aDataLength);
  }
  return aStr.Size();
}


__daim_export cci_result
CCI_StringCopy( dmAString &aDestStr, const dmAString& aSrcStr )
{
  aDestStr.Assign(aSrcStr);
  return CCI_OK;
}


} // extern "C"

