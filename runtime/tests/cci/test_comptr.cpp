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

#define CCI_IFOO_IID \
{ 0x6f7652e0,  0xee43, 0x11d1, \
 { 0x9c, 0xc3, 0x00, 0x60, 0x08, 0x8c, 0xa6, 0xb3 } }

class IFoo : public cciISupports
{
  public:
    CCI_DECLARE_STATIC_IID_ACCESSOR(CCI_IFOO_IID)

  public:
    IFoo();
    // virtual dtor because IBar uses our Release()
    virtual ~IFoo();

    CCI_IMETHOD_(cci_refcnt) AddRef();
    CCI_IMETHOD_(cci_refcnt) Release();
    CCI_IMETHOD QueryInterface( const dmIID&, void** );

    static void print_totals();

    static unsigned int total_constructions();
    static unsigned int total_destructions();

  private:
    unsigned int refcount_;

    static unsigned int total_constructions_;
    static unsigned int total_destructions_;
};

CCI_DEFINE_STATIC_IID_ACCESSOR(IFoo, CCI_IFOO_IID)

class IBar;

  // some types I'll need
typedef unsigned long CCI_RESULT;

// some functions I'll need (and define below)
cci_result  CreateIFoo( void** );
cci_result  CreateIBar( void** result );
void  AnIFooPtrPtrContext( IFoo** );
void  AnISupportsPtrPtrContext( cciISupports** );
void  AVoidPtrPtrContext( void** );
bool  set_a_IFoo( cci_Ptr<IFoo>* result );
cci_Ptr<IFoo>  return_a_IFoo();



unsigned int IFoo::total_constructions_;
unsigned int IFoo::total_destructions_;


class test_message
  {
    public:
      test_message()
        {
          printf("BEGIN unit tests for |cci_Ptr|, compiled " __DATE__ "\n");
        }

     ~test_message()
        {
          IFoo::print_totals();
          printf("END unit tests for |cci_Ptr|.\n");
        }
  };

test_message gTestMessage;

  /*
    ...
  */

void
IFoo::print_totals()
  {
    printf("total constructions/destructions --> %d/%d\n",
           total_constructions_, total_destructions_);
  }

unsigned int
IFoo::total_constructions() { return total_constructions_; }

unsigned int
IFoo::total_destructions()  { return total_destructions_; }


IFoo::IFoo()
    : refcount_(0)
  {
    ++total_constructions_;
    printf("  new IFoo@%p [#%d]\n",
           STATIC_CAST(void*, this), total_constructions_);
  }

IFoo::~IFoo()
  {
    ++total_destructions_;
    printf("IFoo@%p::~IFoo() [#%d]\n",
           STATIC_CAST(void*, this), total_destructions_);
  }

cci_refcnt
IFoo::AddRef()
  {
    ++refcount_;
    printf("IFoo@%p::AddRef(), refcount --> %d\n",
           STATIC_CAST(void*, this), refcount_);
    return refcount_;
  }

cci_refcnt
IFoo::Release()
  {
    int newcount = --refcount_;
    if ( newcount == 0 )
      printf(">>");

    printf("IFoo@%p::Release(), refcount --> %d\n",
           STATIC_CAST(void*, this), refcount_);

    if ( newcount == 0 )
      {
        printf("  delete IFoo@%p\n", STATIC_CAST(void*, this));
        printf("<<IFoo@%p::Release()\n", STATIC_CAST(void*, this));
        delete this;
      }

    return newcount;
  }

cci_result
IFoo::QueryInterface( const dmIID& aIID, void** aResult )
  {
    printf("IFoo@%p::QueryInterface()\n", STATIC_CAST(void*, this));
    cciISupports* rawPtr = 0;
    cci_result status = CCI_OK;

    if ( aIID.Equals(GetIID()) )
      rawPtr = this;
    else
      {
        dmID iid_of_ISupports = CCI_ISUPPORTS_IID;
        if ( aIID.Equals(iid_of_ISupports) )
          rawPtr = STATIC_CAST(cciISupports*, this);
        else
          status = CCI_ERROR_NO_INTERFACE;
      }

    CCI_IF_ADDREF(rawPtr);
    *aResult = rawPtr;

    return status;
  }

cci_result
CreateIFoo( void** result )
    // a typical factory function (that calls AddRef)
  {
    printf(">>CreateIFoo() --> ");
    IFoo* foop = new IFoo;
    printf("IFoo@%p\n", STATIC_CAST(void*, foop));

    foop->AddRef();
    *result = foop;

    printf("<<CreateIFoo()\n");
    return 0;
  }

bool
set_a_IFoo( cci_Ptr<IFoo>* result )
  {
    printf(">>set_a_IFoo()\n");
    if(!result)
       return false;

    cci_Ptr<IFoo> foop( do_QueryInterface(new IFoo) );
    *result = foop;
    printf("<<set_a_IFoo()\n");

    return true;
  }

cci_Ptr<IFoo>
return_a_IFoo()
  {
    printf(">>return_a_IFoo()\n");
    cci_Ptr<IFoo> foop( do_QueryInterface(new IFoo) );
    printf("<<return_a_IFoo()\n");
    return foop;
  }




#define CCI_IBAR_IID \
{ 0x6f7652e1,  0xee43, 0x11d1, \
 { 0x9c, 0xc3, 0x00, 0x60, 0x08, 0x8c, 0xa6, 0xb3 } }

class IBar : public IFoo
  {
    public:
      CCI_DECLARE_STATIC_IID_ACCESSOR(CCI_IBAR_IID)

    public:
      IBar();
      virtual ~IBar();

      CCI_IMETHOD QueryInterface( const dmIID&, void** );
  };

CCI_DEFINE_STATIC_IID_ACCESSOR(IBar, CCI_IBAR_IID)

IBar::IBar()
  {
    printf("  new IBar@%p\n", STATIC_CAST(void*, this));
  }

IBar::~IBar()
  {
    printf("IBar@%p::~IBar()\n", STATIC_CAST(void*, this));
  }

cci_result
IBar::QueryInterface( const dmID& aIID, void** aResult )
  {
    printf("IBar@%p::QueryInterface()\n", STATIC_CAST(void*, this));
    cciISupports* rawPtr = 0;
    cci_result status = CCI_OK;

    if ( aIID.Equals(GetIID()) )
      rawPtr = this;
    else if ( aIID.Equals(CCI_GET_IID(IFoo)) )
      rawPtr = STATIC_CAST(IFoo*, this);
    else
      {
        dmID iid_of_ISupports = CCI_ISUPPORTS_IID;
        if ( aIID.Equals(iid_of_ISupports) )
          rawPtr = STATIC_CAST(cciISupports*, this);
        else
          status = CCI_ERROR_NO_INTERFACE;
      }

    CCI_IF_ADDREF(rawPtr);
    *aResult = rawPtr;

    return status;
  }



cci_result
CreateIBar( void** result )
    // a typical factory function (that calls AddRef)
  {
    printf(">>CreateIBar() --> ");
    IBar* barp = new IBar;
    printf("IBar@%p\n", STATIC_CAST(void*, barp));

    barp->AddRef();
    *result = barp;

    printf("<<CreateIBar()\n");
    return 0;
  }

void
AnIFooPtrPtrContext( IFoo** )
  {
  }

void
AVoidPtrPtrContext( void** )
  {
  }

void
AnISupportsPtrPtrContext( cciISupports** )
  {
  }


// Optimism
#define TEST_EXCEPTIONS 1

// HAVE_CPP_EXCEPTIONS is defined automagically on unix
#if defined(XP_UNIX) || defined(XP_BEOS) || defined(XP_OS2)
#if !defined(HAVE_CPP_EXCEPTIONS)
#undef TEST_EXCEPTIONS
#endif
#endif

#ifdef TEST_EXCEPTIONS
static
cci_result
TestBloat_Raw()
  {
    IBar* barP = 0;
    cci_result result = CreateIBar(REINTERPRET_CAST(void**, &barP));

    if ( barP )
      {
        try
          {
            IFoo* fooP = 0;
            if ( CCI_SUCCEEDED( result = barP->QueryInterface(CCI_GET_IID(IFoo), REINTERPRET_CAST(void**, &fooP)) ) )
              {
                try
                  {
                    fooP->print_totals();
                  }
                catch( ... )
                  {
                    CCI_RELEASE(fooP);
                    throw;
                  }

                CCI_RELEASE(fooP);
              }
          }
        catch( ... )
          {
            CCI_RELEASE(barP);
            result = CCI_ERROR_FAILURE;
          }

        CCI_RELEASE(barP);
      }

    return result;
  }
#endif // TEST_EXCEPTIONS

static
cci_result
TestBloat_Raw_Unsafe()
  {
    IBar* barP = 0;
    cci_result result = CreateIBar(REINTERPRET_CAST(void**, &barP));

    if ( barP )
      {
        IFoo* fooP = 0;
        if ( CCI_SUCCEEDED( result = barP->QueryInterface(CCI_GET_IID(IFoo), REINTERPRET_CAST(void**, &fooP)) ) )
          {
            fooP->print_totals();
            CCI_RELEASE(fooP);
          }

        CCI_RELEASE(barP);
      }

    return result;
  }


static
cci_result
TestBloat_Smart()
  {
    cci_Ptr<IBar> barP;
    cci_result result = CreateIBar( getter_AddRefs(barP) );

    cci_Ptr<IFoo> fooP( do_QueryInterface(barP, &result) );

    if ( fooP )
      fooP->print_totals();

    return result;
  }


cci_Ptr<IFoo> gFoop;

int main( int argc, char ** argv )
{

  DM_BEGIN_TEST_BLOCK

    printf(">>main()\n");

    printf("sizeof(cci_Ptr<IFoo>) --> %lu\n", sizeof(cci_Ptr<IFoo>));

#ifdef TEST_EXCEPTIONS
    printf("============\n");
    DM_TEST_EXPECT_RESULT(CCI_SUCCEEDED(TestBloat_Raw()));
#endif // TEST_EXCEPTIONS

    printf("============\n");
    DM_TEST_EXPECT_RESULT(CCI_SUCCEEDED(TestBloat_Raw_Unsafe()));

    printf("============\n");
    DM_TEST_EXPECT_RESULT(CCI_SUCCEEDED(TestBloat_Smart()));

    printf("============\n");
    IFoo::print_totals();
    DM_TEST_EXPECT_RESULT(IFoo::total_constructions() == IFoo::total_destructions() );

    {
      printf("\n### Test  1: will a |cci_Ptr| call |AddRef| on a pointer assigned into it?\n");
      cci_Ptr<IFoo> foop( do_QueryInterface(new IFoo) );

      printf("\n### Test  2: will a |cci_Ptr| |Release| its old pointer when a new one is assigned in?\n");
      foop = do_QueryInterface(new IFoo);

        // [Shouldn't compile] Is it a compile time error to try to |AddRef| by hand?
      //foop->AddRef();

        // [Shouldn't compile] Is it a compile time error to try to |Release| be hand?
      //foop->Release();

        // [Shouldn't compile] Is it a compile time error to try to |delete| an |cci_Ptr|?
      //delete foop;

      printf("\n### Test  3: can you |AddRef| if you must?\n");
      STATIC_CAST(IFoo*, foop)->AddRef();

      printf("\n### Test  4: can you |Release| if you must?\n");
      STATIC_CAST(IFoo*, foop)->Release();

      printf("\n### Test  5: will a |cci_Ptr| |Release| when it goes out of scope?\n");
    }

    {
      printf("\n### Test  6: will a |cci_Ptr| call the correct destructor?\n");
      cci_Ptr<IFoo> foop( do_QueryInterface(new IBar) );
    }

    {
      printf("\n### Test  7: can you compare one |cci_Ptr| with another [!=]?\n");

      cci_Ptr<IFoo> foo1p( do_QueryInterface(new IFoo) );

        // [Shouldn't compile] Is it a compile time error to omit |getter_[doesnt_]AddRef[s]|?
      //AnIFooPtrPtrContext(&foo1p);

        // [Shouldn't compile] Is it a compile time error to omit |getter_[doesnt_]AddRef[s]|?
      //AVoidPtrPtrContext(&foo1p);

      cci_Ptr<IFoo> foo2p( do_QueryInterface(new IFoo) );

      if ( foo1p != foo2p )
        printf("foo1p != foo2p\n");
      else
        printf("foo1p == foo2p\n");

      DM_TEST_EXPECT_RESULT(foo1p != foo2p);

      printf("\n### Test  7.5: can you compare a |cci_Ptr| with NULL, 0, nsnull [!=]?\n");
      if ( foo1p != 0 )
        printf("foo1p != 0\n");
      if ( 0 != foo1p )
        printf("0 != foo1p\n");
      if ( foo1p == 0 )
        printf("foo1p == 0\n");
      if ( 0 == foo1p )
        printf("0 == foo1p\n");

      DM_TEST_EXPECT_RESULT(foo1p != 0);
      DM_TEST_EXPECT_RESULT(0 != foo1p);

      IFoo* raw_foo2p = foo2p.get();

      printf("\n### Test  8: can you compare a |cci_Ptr| with a raw interface pointer [!=]?\n");
      if ( foo1p.get() != raw_foo2p )
        printf("foo1p != raw_foo2p\n");
      else
        printf("foo1p == raw_foo2p\n");

      DM_TEST_EXPECT_RESULT(foo1p.get() != raw_foo2p);

      printf("\n### Test  9: can you assign one |cci_Ptr| into another?\n");
      foo1p = foo2p;

      printf("\n### Test 10: can you compare one |cci_Ptr| with another [==]?\n");
      if ( foo1p == foo2p )
        printf("foo1p == foo2p\n");
      else
        printf("foo1p != foo2p\n");

      DM_TEST_EXPECT_RESULT(foo1p == foo2p);

      printf("\n### Test 11: can you compare a |cci_Ptr| with a raw interface pointer [==]?\n");
      if ( raw_foo2p == foo2p.get() )
        printf("raw_foo2p == foo2p\n");
      else
        printf("raw_foo2p != foo2p\n");

      DM_TEST_EXPECT_RESULT(raw_foo2p == foo2p.get());

#if 1
      printf("\n### Test 11.5: can you compare a |cci_Ptr| with a raw interface pointer [==]?\n");
      if ( cci_Ptr<IFoo>( raw_foo2p ) == foo2p )
        printf("raw_foo2p == foo2p\n");
      else
        printf("raw_foo2p != foo2p\n");

      DM_TEST_EXPECT_RESULT(cci_Ptr<IFoo>( raw_foo2p ) == foo2p);
#endif

      printf("\n### Test 12: bare pointer test?\n");
      if ( foo1p )
        printf("foo1p is not NULL\n");
      else
        printf("foo1p is NULL\n");

      DM_TEST_EXPECT_RESULT(foo1p);

      printf("\n### Test 13: numeric pointer test?\n");
      if ( foo1p == 0 )
        printf("foo1p is NULL\n");
      else
        printf("foo1p is not NULL\n");

      DM_TEST_EXPECT_RESULT(foo1p != 0);

#if 0
      if ( foo1p == 1 )
        printf("foo1p allowed compare with in\n");
#endif

      printf("\n### Test 14: how about when two |cci_Ptr|s referring to the same object go out of scope?\n");
    }

    {
      printf("\n### Test 15,16 ...setup...\n");
      IFoo* raw_foo1p = new IFoo;
      raw_foo1p->AddRef();

      IFoo* raw_foo2p = new IFoo;
      raw_foo2p->AddRef();

      printf("\n### Test 15: what if I don't want to |AddRef| when I construct?\n");
      cci_Ptr<IFoo> foo1p( dont_AddRef(raw_foo1p) );
      //cci_Ptr<IFoo> foo1p = dont_AddRef(raw_foo1p);

      printf("\n### Test 16: what if I don't want to |AddRef| when I assign in?\n");
      cci_Ptr<IFoo> foo2p;
      foo2p = dont_AddRef(raw_foo2p);
    }

    {
      printf("\n### setup for Test 17\n");
      cci_Ptr<IFoo> foop;
      printf("### Test 17: basic parameter behavior?\n");
      CreateIFoo( cciGetterAddRefs<IFoo>(foop) );
    }
    printf("### End Test 17\n");

    {
      printf("\n### setup for Test 18\n");
      cci_Ptr<IFoo> foop;
      printf("### Test 18: basic parameter behavior, using the short form?\n");
      CreateIFoo( getter_AddRefs(foop) );
    }
    printf("### End Test 18\n");

    {
      printf("\n### setup for Test 19, 20\n");
      cci_Ptr<IFoo> foop;
      printf("### Test 19: reference parameter behavior?\n");
      DM_TEST_EXPECT_RESULT(set_a_IFoo(address_of(foop)));

      printf("### Test 20: return value behavior?\n");
      foop = return_a_IFoo();
    }
    printf("### End Test 19, 20\n");

    {
      printf("\n### setup for Test 21\n");
      cci_Ptr<IFoo> fooP;

      printf("### Test 21: is |QueryInterface| called on assigning in a raw pointer?\n");
      fooP = do_QueryInterface(new IFoo);
    }
    printf("### End Test 21\n");

    {
      printf("\n### setup for Test 22\n");
      cci_Ptr<IFoo> fooP;
      fooP = do_QueryInterface(new IFoo);

      cci_Ptr<IFoo> foo2P;

      printf("### Test 22: is |QueryInterface| _not_ called when assigning in a smart-pointer of the same type?\n");
      foo2P = fooP;
    }
    printf("### End Test 22\n");

    {
      printf("\n### setup for Test 23\n");
      cci_Ptr<IBar> barP( do_QueryInterface(new IBar) );

      printf("### Test 23: is |QueryInterface| called when assigning in a smart-pointer of a different type?\n");

      cci_Ptr<IFoo> fooP( do_QueryInterface(barP) );
      if ( fooP )
        printf("an IBar* is an IFoo*\n");
    }
    printf("### End Test 23\n");

    {
      printf("\n### setup for Test 24\n");
      cci_Ptr<IFoo> fooP( do_QueryInterface(new IFoo) );

      printf("### Test 24: does |forget| avoid an AddRef/Release when assigning to another cci_Ptr?\n");
      cci_Ptr<IFoo> fooP2( fooP.forget() );
    }
    printf("### End Test 24\n");


    {
      cci_Ptr<IFoo> fooP;

      AnIFooPtrPtrContext( getter_AddRefs(fooP) );
      AVoidPtrPtrContext( getter_AddRefs(fooP) );
      AnISupportsPtrPtrContext( getter_AddRefs(fooP) );
    }


    {
      cci_Ptr<cciISupports> supportsP;

      AVoidPtrPtrContext( getter_AddRefs(supportsP) );
      AnISupportsPtrPtrContext( getter_AddRefs(supportsP) );
    }

    // Warning this will lead to false 'leak'
    printf("\n### Test 25: will a static |cci_Ptr| |Release| before program termination?\n");
    gFoop = do_QueryInterface(new IFoo);

  DM_END_TEST_BLOCK

  DM_EXIT_TEST
}
