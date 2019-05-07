/* ::: BEGIN LICENSE BLOCK:::
 * Copyright (c) 2005 David Marteau
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

#include "dm_utilities.h"
#include "dmTraceMem.h"
#include "dmCrt.h"

extern void __dm_ClearEnvironmentTable();
extern void __dm_InitEnvironmentTable();

bool dmUtilities::IsInitialized = false;

#ifndef DM_CONFIG_NOALLOCPROXY
// Hold string so that string allocators will not be released
// Until these strings are released
dmCString gCStringGuard;
dmString  gStringGuard;
#endif

//------------------------------------------
void dmUtilities::Initialize( const char** argv, int argc )
{
  if(!IsInitialized)
  {
    bool _traceMem    = false;
    bool _activateLog = true;

    if(argv)
    {
      for(int i=0;i<argc;++i)
      {
        if(strcmp(argv[i],"--disable-log")==0)
          _activateLog = false;
        else
        if(strcmp(argv[i],"--enable-trace-memory")==0) {
           _traceMem = true;
        }
      }
    }

    if(_traceMem)
       dmTraceMem::Init(0);

    // Initialize environment table
    __dm_InitEnvironmentTable();

    #ifndef DM_CONFIG_NOALLOCPROXY
      gCStringGuard.Assign("I'm The ascii sentinel");
      gStringGuard.Assign(DM_LL("I'm The unicode sentinel"));
    #endif

    if(_activateLog)
       dmException::ActivateLog();

    IsInitialized = true;
  }
}
//------------------------------------------
void dmUtilities::Finalize()
{
  if(IsInitialized)
  {
     __dm_ClearEnvironmentTable();

    #ifndef DM_CONFIG_NOALLOCPROXY
      gCStringGuard.Release();
      gStringGuard.Release();
    #endif

    IsInitialized = false;
  }

#ifdef DM_ENABLE_TRACE_MEM
  if(dmTraceMem::IsTracing()) {
     dmTraceMem::Dump(1);
  }
#endif
}
//------------------------------------------
