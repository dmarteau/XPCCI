#ifndef cciCOM_h
#define cciCOM_h

#ifndef cciCore_h
#include "cciCore.h"
#endif

class cciIServiceManager;
class cciIComponentManager;
class cciIModule;

typedef cci_result (*cciGetModuleProc)( cciIComponentManager*, const char* location,
                                        cciIModule** result);

typedef struct cciStaticModuleInfo
{
  const char*       name;
  cciGetModuleProc  getModule;
} cciStaticModuleInfo;

extern "C" {

  __daim_export cci_result __daim_call( CCI_Init )( cciIServiceManager* *result,
                                                    cciStaticModuleInfo const *aStaticModules,
                                                    dm_uint32 aStaticModuleCount,
                                                    bool registerStaticModules);

  __daim_export cci_result __daim_call( CCI_Shutdown )(cciIServiceManager* cmpMgr);

}

#define CCI_COMPONENTS_PATH_ENV "CCI_COMPONENTS"

#endif // cciCOM_h
