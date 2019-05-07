#ifndef cciStubs_h
#define cciStubs_h

class cciIServiceManager;
class cciIComponentManager;
class cciIComponentRegistrar;
class cciIAllocator;

typedef CCI_CALLBACK_(void,CCI_STATIC_DESTRUCTOR)(void*);

extern "C" {

__daim_export void*       __daim_call( CCI_NewObject    )(size_t n, CCI_STATIC_DESTRUCTOR );
__daim_export void        __daim_call( CCI_RevokeObject )(void* p , bool discard );
__daim_export cci_result  __daim_call( CCI_VerifyObject )(void* p);
__daim_export cci_result  __daim_call( CCI_NewAllocator )(cciIAllocator* *result);
__daim_export cci_result  __daim_call( CCI_GetComponentManager  )(cciIComponentManager* *result);
__daim_export cci_result  __daim_call( CCI_GetServiceManager    )(cciIServiceManager*   *result);
__daim_export cci_result  __daim_call( CCI_GetComponentRegistrar)(cciIComponentRegistrar* *result);

}

#endif // cciStubs_h
