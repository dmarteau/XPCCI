
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

#include "cciCOM.h"
#include "cciCOMPtr.h"
#include "cciString.h"
#include "cciIServiceManager.h"
#include "cciIComponentRegistrar.h"
#include "xpcci.h"
#include "dmCrt.h"

// Static modules

#define DECLARE_STATIC_MODULE(name) cci_result         \
  name##_CCIGetModule(cciIComponentManager *servMgr,   \
                      const char*  location,           \
                      cciIModule** result)

DECLARE_STATIC_MODULE(StreamModule);

#define STATIC_MODULE_INFO(name) \
  { #name, name##_CCIGetModule }

static cciStaticModuleInfo gStaticModules[] =
{
  STATIC_MODULE_INFO(StreamModule),
};


/**
 * DAIM API
 */

static cci_refcnt  gDaimRefcount = 0;

static cci_result RegisterComponents( cciIServiceManager* srvmngr, const char* registryLocation )
{
  cci_result rv;

  dmLOG("INIT: Loading persistent registry at %s",registryLocation);
  cci_Ptr<cciIComponentRegistrar> compReg = do_QueryInterface(srvmngr,&rv);
  if(CCI_SUCCEEDED(rv))
    rv = compReg->ReadPersistentRegistry(registryLocation);

  return rv;
}

extern "C" {

__daim_export cci_result
DM_Initialize( const char** _argv, int _argc, dmLOG_FUNCTION pfnLog )
{
  // Prevent any subsequent initialization
  if(++gDaimRefcount>=2)
     return CCI_ERROR_ALREADY_INITIALIZED;

  cci_result rv;

  dmTRY

    dmException::SetLogFunction( pfnLog );

    dmUtilities::Initialize(_argv,_argc);

    cciIServiceManager* srvmngr;

    dmCString registryLocation;

    // Start the CCI/COM framework
    rv = CCI_Init(&srvmngr,gStaticModules,DM_ARRAY_LENGTH(gStaticModules),true);

    // Register native components
    if(CCI_SUCCEEDED(rv) && _argv)
    {
      for(int i=0;i<_argc;++i)
      {
        if(_dm_strcmp(_argv[i],"--registry-location")==0 && i<(_argc-1))
        {
          registryLocation = _argv[i+1];
        } else
        if(_dm_strcmp(_argv[i],"--daim-bin")==0 && i<(_argc-1))
        {
          dmPortability::SetEnv("XPCCI_BIN",_argv[i+1]);
        }
      }
    }

    // Check if XPCCI_BIN is defined in environment
    // Add path to components to the CCI_COMPONENTS_PATH_ENV variables

    dmCString daimLibraryPath = dmPortability::GetEnv("XPCCI_BIN");
    if(!daimLibraryPath.IsEmpty())
    {
      dmCString cciComponentPath = daimLibraryPath;
      cciComponentPath.Append("/cci");

      // Append to component paths
      dmPortability::SetEnv(CCI_COMPONENTS_PATH_ENV,
                            dmPortability::GetEnv(CCI_COMPONENTS_PATH_ENV)
                           .Append(dmPortability::PathEnvSeparator())
                           .Append(cciComponentPath).get());

      // Assign default registry location if not already defined
      if(registryLocation.IsEmpty())
      {
        registryLocation = cciComponentPath;
        registryLocation.Append("/ccireg.dat");
      }
    }

    // Load registry if specified
    if(!registryLocation.IsEmpty() &&
        dmPortability::AccessFile(registryLocation.get(),dmPortability::eEXISTS))
    {
       rv = RegisterComponents(srvmngr,registryLocation.get());
    }

    CCI_IF_RELEASE(srvmngr);

    dmCATCH(_E)
    dmTRACE_EXCEPTION(_E);
    rv = CCI_ERROR_FAILURE;
  dmDONE

  return rv;
}

__daim_export void
DM_Finalize( bool force )
{
  if(force || --gDaimRefcount<=0)
  {
    dmTRY
      CCI_Shutdown(dm_null);
      dmException::SetLogFunction( NULL );
      dmUtilities::Finalize();
    dmCATCH(_E)
      dmTRACE_EXCEPTION(_E);
    dmDONE
  }
}

} // extern "C"
