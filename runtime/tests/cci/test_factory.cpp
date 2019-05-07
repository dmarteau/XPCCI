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
// File         : test_comptr.cpp
// Date         : 28 nov. 2008
// Author       : David Marteau
//--------------------------------------------------------

#include "test_common.h"

#include "cciCOMPtr.h"
#include "cciComponentManagerUtils.h"
#include "cciServiceManagerUtils.h"

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

static const cciModuleComponentInfo components[] = {
    {  "Foo module",
       CCI_FOO_CID,
       CCI_FOO_CONTRACTID,
       FooConstructor
    },
};

//==================================================

class test_message
  {
    public:
      test_message()
        {
          printf("BEGIN unit tests for cci factory, compiled " __DATE__ "\n");
        }

     ~test_message()
        {
          printf("END unit tests for for cci factory.\n");
        }
  };

test_message gTestMessage;

int main( int argc, char ** argv )
{
  DM_BEGIN_TEST_BLOCK

    printf(">>main()\n");

    CCI_REGISTER_FACTORY_COMPONENTS(components);

    {
      printf("### Test 1: component manager\n");
      cci_Ptr<IFoo> foo = do_CreateInstance(CCI_FOO_CONTRACTID);

      DM_TEST_EXPECT_RESULT(foo);
      DM_TEST_EXPECT_RESULT(CCI_SUCCEEDED(foo->TestMe()));
    }

    {
      printf("### Test 2: service manager\n");
      cci_Ptr<IFoo> foo = do_GetService(CCI_FOO_CONTRACTID);
      DM_TEST_EXPECT_RESULT(foo);
      DM_TEST_EXPECT_RESULT(CCI_SUCCEEDED(foo->TestMe()));

      cci_Ptr<IFoo> foo2 = do_GetService(CCI_FOO_CONTRACTID);
      DM_TEST_EXPECT_RESULT(foo == foo2);
    }

  DM_END_TEST_BLOCK

  DM_EXIT_TEST
}
