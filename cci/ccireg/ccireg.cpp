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
// File         : compreg.cpp
// Date         : 1 déc. 2008
// Author       : David Marteau
//--------------------------------------------------------

#include "cciCOM.h"
#include "cciCOMPtr.h"
#include "cciIServiceManager.h"
#include "cciIComponentRegistrar.h"

#include "dm_utilities.h"

#include <stdio.h>
#include <string.h>

// Static modules
/*
#define DECLARE_STATIC_MODULE(name) cci_result         \
  name##_CCIGetModule(cciIComponentManager *servMgr,   \
                      const char*  location,           \
                      cciIModule** result)

#define STATIC_MODULE_INFO(name) \
  { #name, name##_CCIGetModule }

static cciStaticModuleInfo gStaticModules[] =
{
};
*/
////////////////////////////////////////////////////////

static const char* mArgv[] = {
  "--enable-trace-memory"
};

static const char usage[] = "ccireg registryfile entry [entry2 ...]";
static const char help[]  =
  "ccireg, compiled " __DATE__ "\n"
  "Create a registry file for CCI modules.\n"
  "Modules will be searched using the " CCI_COMPONENTS_PATH_ENV " environment variable\n";

int main( int argc, char ** argv )
{
  if(argc<3) {
    printf("%s\nUsage : %s\n",help,usage);
    return 1;
  }

  const char* registryLocation = argv[1];

  cci_result rv;

  dmTRY

    cciIServiceManager* srvmngr;

    dmUtilities::Initialize(mArgv,sizeof(mArgv)/sizeof(char*));

    // Start the CCI/COM framework
    rv = CCI_Init(&srvmngr,dm_null,dm_null,false);
    if(CCI_SUCCEEDED(rv))
    {
      if(argc>1)
      {
        cci_Ptr<cciIComponentRegistrar> cmpreg = do_QueryInterface(srvmngr,&rv);
        if(cmpreg)
        {
          for(int i=2;i<argc;++i)
          {
            // Load the component
            rv = cmpreg->AutoRegister(argv[i]);
            if(CCI_FAILED(rv))
              dmLOG_ERROR("Failed to register %s",argv[i]);
          }

          rv = cmpreg->WritePersistentRegistry(registryLocation);
          if(CCI_FAILED(rv))
            dmLOG_ERROR("Failed to write registry !");
        }
        else
        {
          dmLOG_ERROR("Cannot get component registrar !");
        }
      }


      CCI_Shutdown(srvmngr);
    }

    dmUtilities::Finalize();

  dmCATCH(_E)
    dmTRACE_EXCEPTION(_E);
    rv = CCI_ERROR_FAILURE;
  dmDONE

  return CCI_FAILED(rv) ? 2 : 0;
}


