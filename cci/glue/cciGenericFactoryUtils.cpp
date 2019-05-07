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
// File         : cciGenericFactoryUtils.cpp
// Date         : 19 nov. 2008
// Author       : David Marteau
//--------------------------------------------------------

#include "cciCOMPtr.h"
#include "cciIGenericFactory.h"
#include "cciIComponentRegistrar.h"


CCI_COM_GLUE cci_result
CCI_RegisterFactoryComponents( const cciModuleComponentInfo *components, dm_uint32 count )
{
   // Get the component registrar
  cciIComponentRegistrar* registrar;

  cci_result rv = CCI_GetComponentRegistrar(&registrar);
  if (CCI_FAILED(rv))
      return rv;

  const cciModuleComponentInfo* desc = components;
  for (dm_uint32 i = 0; i < count; i++)
  {
    if (desc->mConstructor)
    {
      cci_Ptr<cciIGenericFactory> fact;
      cci_result rv = CCI_NewGenericFactory(getter_AddRefs(fact), desc);
      if(CCI_FAILED(rv))
         return rv;

      rv = registrar->RegisterFactory(desc->mCID,
                                      desc->mDescription,
                                      desc->mContractID,
                                      fact);
      if(CCI_FAILED(rv))
         break;
    }
    desc++;
  }

  CCI_IF_RELEASE(registrar);
  return rv;
}


