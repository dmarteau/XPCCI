#ifndef cciPrivateHook_h
#define cciPrivateHook_h

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

#define CCI_HOOK_VERSION 1

struct cciModuleInfo;
struct cciModuleComponentInfo;

class cciIAllocator;
class cciIModule;
class cciIGenericFactory;

extern "C" {

typedef cci_result (* cci_GetServiceManagerProc)(cciIServiceManager* *result);
typedef cci_result (* cci_GetComponentManagerProc)(cciIComponentManager* *result);
typedef cci_result (* cci_GetComponentRegistrarProc)(cciIComponentRegistrar* *result);
typedef cci_result (* cci_NewGenericModuleProc)(struct cciModuleInfo*,cciIModule* *result);
typedef cci_result (* cci_NewGenericFactoryProc)(cciIGenericFactory**, const cciModuleComponentInfo*);
typedef cci_result (* cci_NewAllocatorProcPtr)(cciIAllocator**);

typedef void*  (* cci_MemAllocProc)(size_t size);
typedef void   (* cci_MemFreeProc)(void* memblock);
typedef void*  (* cci_MemReallocProc)(void *memblock, size_t size );
typedef void*  (* cci_MemCloneProc)(const void *src, size_t size );

typedef void   (* cci_DebugBreakProc)(dm_uint32,const char *aStr, const char *aExpr,
                                                const char *aFile, dm_uint32 aLine);

typedef cci_result  (* cci_CStringContainerInitProc   )(cciCStringContainer &aContainer);
typedef void        (* cci_CStringContainerFinishProc )(cciCStringContainer &aContainer);
typedef cci_result  (* cci_CStringSetDataProc)(dmACString &aStr, const char  *aData, dm_uint32 aDataLength);
typedef dm_uint32   (* cci_CStringGetDataProc)(const dmACString &aStr, const char **aData );
typedef cci_result  (* cci_CStringAppendDataProc)(dmACString &aStr, const char *aData, dm_uint32 aDataLength );
typedef cci_result  (* cci_CStringCopyProc)(dmACString &aDestStr, const dmACString& aSrcStr );

typedef cci_result  (* cci_StringContainerInitProc   )(cciStringContainer &aContainer);
typedef void        (* cci_StringContainerFinishProc )(cciStringContainer &aContainer);
typedef cci_result  (* cci_StringSetDataProc)(dmAString &aStr, const dm_unichar  *aData, dm_uint32 aDataLength);
typedef dm_uint32   (* cci_StringGetDataProc)(const dmAString &aStr, const dm_unichar **aData );
typedef cci_result  (* cci_StringAppendDataProc)(dmAString &aStr, const dm_unichar *aData, dm_uint32 aDataLength );
typedef cci_result  (* cci_StringCopyProc)(dmAString &aDestStr, const dmAString& aSrcStr );

struct cciPrivateHook
{
  dm_uint32                      version;

  cci_GetServiceManagerProc      getServiceManagerProcPtr;
  cci_GetComponentManagerProc    getComponentManagerProcPtr;
  cci_GetComponentRegistrarProc  getComponentRegistrarProcPtr;

  cci_NewGenericModuleProc       newGenericModuleProcPtr;
  cci_NewGenericFactoryProc      newGenericFactoryProcPtr;
  cci_NewAllocatorProcPtr        newAllocatorProcPtr;

  cci_MemAllocProc               memAllocProcPtr;
  cci_MemFreeProc                memFreeProcPtr;
  cci_MemReallocProc             memReallocProcPtr;
  cci_MemCloneProc               memCloneProcPtr;

  cci_DebugBreakProc             debugBreakProcPtr;

  cci_CStringContainerInitProc   cstringContainerInitProcPtr;
  cci_CStringContainerFinishProc cstringContainerFinishProcPtr;
  cci_CStringSetDataProc         cstringSetDataProcPtr;
  cci_CStringGetDataProc         cstringGetDataProcPtr;
  cci_CStringAppendDataProc      cstringAppendDataProcPtr;
  cci_CStringCopyProc            cstringCopyProcPtr;

  cci_StringContainerInitProc    stringContainerInitProcPtr;
  cci_StringContainerFinishProc  stringContainerFinishProcPtr;
  cci_StringSetDataProc          stringSetDataProcPtr;
  cci_StringGetDataProc          stringGetDataProcPtr;
  cci_StringAppendDataProc       stringAppendDataProcPtr;
  cci_StringCopyProc             stringCopyProcPtr;
};


struct cciModuleHook;

typedef void       (*cciModuleCloseProc) (struct cciModuleHook* );
typedef cci_result (*cciModuleInitProc)  (struct cciModuleHook*, const char* location,
                                          cciIModule** _result);

struct cciModuleTag  {
  // Filled by the module itself
  dm_uint32          ccih_Magic;            // Magic number
  dm_uint32          ccih_TagVersion;       // Module version
  cciModuleInitProc  ccih_Init;             // Custom module Init function callback
  cciModuleCloseProc ccih_Close;            // Custom module Close function callback
};

//--------------------------------------------------------
struct cciModuleHook {
  struct cciModuleTag*    cc_Tag;
  struct cciPrivateHook*  cc_Hooks;
  dm_hmodule              cc_Library;
};
//--------------------------------------------------------

CCI_COM_GLUE cci_result CCI_InitCOMGlue2( cciPrivateHook * pHook );
CCI_COM_GLUE void       CCI_ReleaseCOMGlue2();

//--------------------------------------------------------

} // extern "C"



#endif // cciPrivateHook_h
