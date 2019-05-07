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
// File         : test_catmngr.cpp
// Date         : 28 nov. 2008
// Author       : David Marteau
//--------------------------------------------------------

#include "test_common.h"

#include "cciCOMPtr.h"
#include "cciComponentManagerUtils.h"
#include "cciServiceManagerUtils.h"
#include "cciICategoryManager.h"
#include "cciIStringEnumerator.h"
#include "cciString.h"

CCI_USE_NS

//==============================
// Test cci COM pointers
//==============================

#define STATIC_CAST(T,x)  static_cast<T>(x)
#define REINTERPRET_CAST(T,x) reinterpret_cast<T>(x)

// BBBD37F5-8F77-4773-851B-A72375CF5EA1
#define CCI_IFOO_IID \
{ 0xBBBD37F5,  0x8F77, 0x4773, \
 { 0x85, 0x1B, 0xA7, 0x23, 0x75, 0xCF, 0x5E, 0xA1 } }



class CCI_NO_VTABLE IFoo : public cciISupports
{
  public:
    CCI_DECLARE_STATIC_IID_ACCESSOR(CCI_IFOO_IID)

    CCI_IMETHOD TestMe() = 0;

};

CCI_DEFINE_STATIC_IID_ACCESSOR(IFoo, CCI_IFOO_IID)

//07CDB964-BFE3-40F2-BD07-6ADAF168940B
#define CCI_IBAR_IID \
{ 0x07CDB964,  0xBFE3, 0x40F2, \
 { 0xBD, 0x07, 0x6A, 0xDA, 0xF1, 0x68, 0x94, 0x0B } }


class Foo final : public IFoo
{
  public:
    CCI_DECL_ISUPPORTS

    Foo() {
      printf("  new Foo@%p\n", STATIC_CAST(void*, this));
    }

    CCI_IMETHOD TestMe() override;

  private:
   ~Foo() {
     printf("  Foo@%p::~Foo()\n", STATIC_CAST(void*, this));
   }

  protected:
};


/* Implementation file */
CCI_IMPL_ISUPPORTS1(Foo, IFoo)


CCI_IMETHODIMP  Foo::TestMe()
{
  return CCI_OK;
}


//=====================================
// Module
//=====================================

#include "cciIGenericFactory.h"

#define CCI_FOO_CID \
{ /* 10B784E6-74CC-41DB-8815-513565818280 */       \
  0x10B784E6,                                      \
  0x74CC,                                          \
  0x41DB,                                          \
  {0x88, 0x15, 0x51, 0x35, 0x65, 0x81, 0x82, 0x80} \
}

#define CCI_FOO_CONTRACTID  "@daim.org/tests/foo;1"

CCI_GENERIC_FACTORY_CONSTRUCTOR(Foo)


static CCI_METHOD RegisterSelfProc(cciIComponentManager *aCompMgr,
                                            const char    *aPath,
                                            const char    *aLoaderStr,
                                            const char    *aType,
                                            const cciModuleComponentInfo *aInfo)
{
  printf("==> Registering foo %s,%s,%s\n",aPath,aLoaderStr,aType);

  // Register the proxy to the category manager
  cci_Ptr<cciICategoryManager> catmngr = do_GetService("@daim.org/category-manager;1");
  if(catmngr)
     return catmngr->AddCategoryEntry("test-category","foo",CCI_FOO_CONTRACTID,true,true);
  else
    printf("==> Huu ?! no category manager !!\n");

  return CCI_ERROR_FAILURE;
}

static CCI_METHOD UnRegisterSelfProc(cciIComponentManager *aCompMgr,
                                     const char *aPath,
                                     const char *aLoaderStr,
                                     const cciModuleComponentInfo *aInfo)
{
  printf("==> Unregistering foo %s,%s\n",aPath,aLoaderStr);

  // Register the proxy to the category manager
  cci_Ptr<cciICategoryManager> catmngr = do_GetService("@daim.org/category-manager;1");
  if(catmngr)
     catmngr->DeleteCategoryEntry("test-category","foo",true);
  else
    printf("==> Huu ?! no category manager !!\n");

  return CCI_OK;
}


static const cciModuleComponentInfo components[] = {
    {  "Foo module",
       CCI_FOO_CID,
       CCI_FOO_CONTRACTID,
       FooConstructor,
       RegisterSelfProc,
       UnRegisterSelfProc
    },
};

//==================================================

class test_message
  {
    public:
      test_message()
        {
          printf("BEGIN unit tests for category manager, compiled " __DATE__ "\n");
        }

     ~test_message()
        {
          printf("END unit tests for for category manager.\n");
        }
  };

test_message gTestMessage;

int main( int argc, char ** argv )
{
  DM_BEGIN_TEST_BLOCK

    printf(">>main()\n");

    {
      cci_result rv;

      printf("### Test 1: Do I have a category manager ?\n");
      cci_Ptr<cciICategoryManager> catmngr = do_GetService("@daim.org/category-manager;1",&rv);
      DM_TEST_EXPECT_RESULT(CCI_SUCCEEDED(rv));

      printf("### Test 1.1: Adding categories...");
      rv = catmngr->AddCategoryEntry("cat1","entry1","value1",true,true);
      DM_TEST_EXPECT_RESULT(CCI_SUCCEEDED(rv));

      rv = catmngr->AddCategoryEntry("cat1","entry2","value2",true,true);
      DM_TEST_EXPECT_RESULT(CCI_SUCCEEDED(rv));

      rv = catmngr->AddCategoryEntry("cat2","entry1","value1",true,true);
      DM_TEST_EXPECT_RESULT(CCI_SUCCEEDED(rv));

      {
        printf("### Test 1.2: Can I enumerate categories ?\n");

        cci_Ptr<cciIUTF8StringEnumerator> categories;

        rv = catmngr->EnumerateCategories(getter_AddRefs(categories));
        DM_TEST_EXPECT_RESULT(CCI_SUCCEEDED(rv));

        dmCString contractid;
        while(categories->HasMore())
        {
          rv = categories->GetNext(contractid);
          DM_TEST_EXPECT_RESULT(CCI_SUCCEEDED(rv));

          printf("==> Found Category: %s\n",contractid.get());
        }
      }

      {
        printf("### Test 1.3: Can I enumerate category entries ?\n");

        cci_Ptr<cciIUTF8StringEnumerator> category;

        const char cat1[] = "cat1";

        rv = catmngr->EnumerateCategory(cat1,getter_AddRefs(category));
        DM_TEST_EXPECT_RESULT(CCI_SUCCEEDED(rv));

        dmCString entry;
        while(category->HasMore())
        {
          rv = category->GetNext(entry);
          DM_TEST_EXPECT_RESULT(CCI_SUCCEEDED(rv));

          printf("==> Found Category entry : %s , %s\n",cat1,entry.get());
        }
      }

      printf("### Test 2: registering foo component\n");
      CCI_REGISTER_FACTORY_COMPONENTS(components);
    }
  DM_END_TEST_BLOCK

  DM_EXIT_TEST
}
