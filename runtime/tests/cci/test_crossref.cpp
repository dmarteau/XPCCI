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


class IBar;

class CCI_NO_VTABLE IFoo : public cciISupports
{
  public:
    CCI_DECLARE_STATIC_IID_ACCESSOR(CCI_IFOO_IID)

    CCI_IMETHOD GetIBar( IBar **_retval ) = 0;
    CCI_IMETHOD SetIBar( IBar *aIBar )    = 0;

};

CCI_DEFINE_STATIC_IID_ACCESSOR(IFoo, CCI_IFOO_IID)

//07CDB964-BFE3-40F2-BD07-6ADAF168940B
#define CCI_IBAR_IID \
{ 0x07CDB964,  0xBFE3, 0x40F2, \
 { 0xBD, 0x07, 0x6A, 0xDA, 0xF1, 0x68, 0x94, 0x0B } }


class CCI_NO_VTABLE IBar : public cciISupports
{
  public:
    CCI_DECLARE_STATIC_IID_ACCESSOR(CCI_IBAR_IID)

    CCI_IMETHOD GetIFoo( IFoo **_retval ) = 0;
    CCI_IMETHOD SetIFoo( IFoo *aIFoo )    = 0;
};

CCI_DEFINE_STATIC_IID_ACCESSOR(IBar, CCI_IBAR_IID)

class Bar final : public IBar
{
  public:
    CCI_DECL_ISUPPORTS

    Bar() {
      printf("  new Bar@%p\n", STATIC_CAST(void*, this));
    }

    CCI_IMETHOD GetIFoo( IFoo **_retval ) override;
    CCI_IMETHOD SetIFoo( IFoo *aIFoo ) override;

  private:
    ~Bar() {
      printf("  Bar@%p::~Bar()\n", STATIC_CAST(void*, this));
    }

  protected:
    cci_Ptr<IFoo> mFoo;

};

/* Implementation file */
CCI_IMPL_ISUPPORTS1(Bar, IBar)


CCI_IMETHODIMP  Bar::GetIFoo( IFoo **_retval )
{
  CCI_IF_ADDREF( *_retval = mFoo );
  return CCI_OK;
}

CCI_IMETHODIMP  Bar::SetIFoo( IFoo *aIFoo )
{
  mFoo = aIFoo;
  return CCI_OK;
}


class Foo final : public IFoo
{
  public:
    CCI_DECL_ISUPPORTS

    Foo() {
      printf("  new Foo@%p\n", STATIC_CAST(void*, this));
    }

    CCI_IMETHOD GetIBar( IBar **_retval ) override;
    CCI_IMETHOD SetIBar( IBar *aIBar ) override;

  private:
   ~Foo() {
     printf("  Foo@%p::~Foo()\n", STATIC_CAST(void*, this));
   }

  protected:
    cci_Ptr<IBar> mBar;
};


/* Implementation file */
CCI_IMPL_ISUPPORTS1(Foo, IFoo)


CCI_IMETHODIMP  Foo::GetIBar( IBar **_retval )
{
  CCI_IF_ADDREF( *_retval = mBar );
  return CCI_OK;
}

CCI_IMETHODIMP  Foo::SetIBar( IBar *aIBar )
{
  mBar = aIBar;
  return CCI_OK;
}


cci_result
CreateIFoo( void** result )
    // a typical factory function (that calls AddRef)
  {
    printf(">>CreateIFoo() --> ");
    IFoo* foop = new Foo;
    printf("IFoo@%p\n", STATIC_CAST(void*, foop));

    CCI_ADDREF(foop);
    *result = foop;

    printf("<<CreateIFoo()\n");
    return 0;
  }

cci_result
CreateIBar( void** result )
    // a typical factory function (that calls AddRef)
  {
    printf(">>CreateIBar() --> ");
    IBar* barp = new Bar;
    printf("IBar@%p\n", STATIC_CAST(void*, barp));

    CCI_ADDREF(barp);
    *result = barp;

    printf("<<CreateIBar()\n");
    return 0;
  }

class test_message
  {
    public:
      test_message()
        {
          printf("BEGIN unit tests for cross reference GC, compiled " __DATE__ "\n");
        }

     ~test_message()
        {
          printf("END unit tests for for cross reference GC.\n");
        }
  };

test_message gTestMessage;

int main( int argc, char ** argv )
{
  DM_BEGIN_TEST_BLOCK

    printf(">>main()\n");

    {
      cci_Ptr<IBar> bar;
      cci_Ptr<IFoo> foo;

      CreateIBar(getter_AddRefs(bar));
      CreateIFoo(getter_AddRefs(foo));

      bar->SetIFoo(foo);
      foo->SetIBar(bar);
    }


  DM_END_TEST_BLOCK

  DM_EXIT_TEST
}
