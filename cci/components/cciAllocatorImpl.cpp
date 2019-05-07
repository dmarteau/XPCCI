
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

#include "cciPrivate.h"
#include "cciCOMPtr.h"
#include "cciAllocatorImpl.h"
#include "cciComponentManager.h"

CCI_USE_NS
//--------------------------------------------------------
// Utility macros
//--------------------------------------------------------

enum { CCI_MAGIC_NUMBER = dm_make_id('C','C','I','O') };

//--------------------------------------------------------

struct cci_ObjectImpl;

struct cci_ObjectHandle : public dmNode {
  cci_ObjectImpl *o_Object;
};

// Private handle on an Object
struct cci_ObjectImpl {
    dm_uint                o_Magic;
    cci_ObjectHandle       o_Handle;
    void*                  o_Owner;
    CCI_STATIC_DESTRUCTOR  o_Destructor;
    size_t                 o_Size;
	//... Instance data follows
};


// Get a pointer to  real object from cci_ObjectImpl address
inline void* CCI_HANDLE_TO_OBJECT( cci_ObjectImpl* _obj ) {
  return _obj+1;
}

// Get a pointer to  'cci_ObjectImpl' from real object pointer
inline cci_ObjectImpl* CCI_OBJECT_TO_HANDLE( void* o ) {
  return reinterpret_cast<struct cci_ObjectImpl*>(o) - 1;
}

#define CCI_ENSURE_MAGIC_(o)  ((o)->o_Magic==CCI_MAGIC_NUMBER)

#define CCI_ENSURE_MAGIC(o) CCI_ENSURE_MAGIC_(CCI_OBJECT_TO_HANDLE(o))

#define CCI_SIZEOF_INSTANCE(size) ( (size) + sizeof (struct cci_ObjectImpl ) )

#define ALLOC_OBJ( size ) static_cast<cci_ObjectImpl*>( dmMemory::Malloc(CCI_SIZEOF_INSTANCE(size)) )
#define FREE_OBJ( ob  ) dmMemory::Free(ob)

#define ROTTED_VALUE 0xDEADBEEF
#define MARK_DEAD_BYTES( loc, n ) \
  DM_BEGIN_MACRO  \
    dm_uint32* p = static_cast<dm_uint32*>(loc); \
    for(int i = n >> 2;--i>=0;) \
      *p++ = ROTTED_VALUE; \
  DM_END_MACRO
//---------------------------------------------------------

typedef struct cciAllocatorState {
  cciAllocatorBase  mAllocator;
  dmList            mGarbageList;
} cciAllocatorState;

static cciAllocatorState* gAllocatorState = dm_null;

cci_result cciAllocatorBase::InitAllocatorState()
{
  CCI_ENSURE_FALSE(gAllocatorState,CCI_ERROR_ALREADY_INITIALIZED);

  gAllocatorState = new (dm_arena) cciAllocatorState;
  if(!gAllocatorState)
     return CCI_ERROR_OUT_OF_MEMORY;

  return CCI_OK;
}

void cciAllocatorBase::ReleaseAllocatorState()
{
  if(gAllocatorState)
  {
    gAllocatorState->mAllocator.FreeAllObjects();

    FreeGarbageList();

    // Need to explicitely call the destructor
    gAllocatorState->~cciAllocatorState();
    
    ::operator delete [](gAllocatorState,dm_arena);
    
  // delete (dm_arena) gAllocatorState;
    gAllocatorState = dm_null;
  }
}

void cciAllocatorBase::FreeGarbageList()
{
  if(!gAllocatorState)
     return;

  cci_ObjectHandle *oh;

  dmList& garbageList = gAllocatorState->mGarbageList;

  dmLOG("Freeing allocators garbage list\n");

  while(!garbageList.Empty() )
  {
    oh = static_cast<cci_ObjectHandle*>(garbageList.Pop_Back());
    cci_ObjectImpl* _object = oh->o_Object;

    dmDestroy(oh);
    FREE_OBJ(_object); // FreeMemory
  }
}

//---------------------------------------------------------

cciAllocatorBase::cciAllocatorBase()
: mNuminsts(0)
{
}

cciAllocatorBase::~cciAllocatorBase()
{
  FreeAllObjects();
}

void cciAllocatorBase::FreeAllObjects()
{
  cci_ObjectHandle *oh;

#ifdef DEBUG
  if(!mInstances.Empty())
     dmLOG("Collecting pending objects...\n");
#endif

  while(!mInstances.Empty())
  {
    oh = static_cast<cci_ObjectHandle*>(mInstances.Pop_Back());

    cci_ObjectImpl *impl = oh->o_Object;

    void* obj = CCI_HANDLE_TO_OBJECT(impl);

    // !!IMPORTANT Revoke the object first
    RevokeObject( obj, false );

    // Force call to destructor
    if(impl->o_Destructor)
       impl->o_Destructor(obj);
    
    // Munge data so that it's going to mess up any
    // held reference
    MARK_DEAD_BYTES(obj,impl->o_Size);
  }

  mNuminsts = 0;
}

//=============================
// cciIAllocator
//=============================

void*
cciAllocatorBase::NewObject(  size_t n, CCI_STATIC_DESTRUCTOR _dtor )
{
  dmCRITICAL_SECTION( mLock );

  size_t _allocated = dm_align32(n);
  
  cci_ObjectImpl*  _object = ALLOC_OBJ( _allocated );

  if(!_object)
	  return dm_null;

  // Construct the handle
  dmConstruct(&_object->o_Handle);

  // Set it's own address so that we can get it back
  // through the object handle

  _object->o_Handle.o_Object  = _object;

  _object->o_Owner      = this;
  _object->o_Magic      = CCI_MAGIC_NUMBER;
  _object->o_Destructor = _dtor;
  _object->o_Size       = _allocated;

  mInstances.Push_Back(&_object->o_Handle);
  mNuminsts++;

  return CCI_HANDLE_TO_OBJECT(_object);
}

cci_result
cciAllocatorBase::RevokeObject( void* object ,bool discard )
{
  CCI_ENSURE_ARG_POINTER(object);

  cci_ObjectImpl* _object = CCI_OBJECT_TO_HANDLE(object);

  if(!CCI_ENSURE_MAGIC_(_object))
    return CCI_ERROR_INVALID_POINTER;

  if(_object->o_Owner != this)
    return CCI_ERROR_UNEXPECTED;

  dmCRITICAL_SECTION( mLock );

  if(discard)
  {
    // Node will remove itself from list
    dmDestroy(&_object->o_Handle);
    FREE_OBJ(_object); // FreeMemory
  }
  else
  {
    // Keep the object in garbage list, this can occur when a object is still
    // handled by someone

    _object->o_Magic = 0; // Remove the magic - Mark as invalid

    cci_ObjectHandle* oh = &_object->o_Handle;
    mInstances.Remove(oh); // Remove from main class list

    CCI_WARNING("Object moved to garbage list\n");

    gAllocatorState->mGarbageList.Push_Back(oh);
  }

  --mNuminsts;

  return CCI_OK;
}

dm_uint32
cciAllocatorBase::NumberOfInstances()
{
   return mNuminsts;
};

bool
cciAllocatorBase::IsObjectValid(void* p)
{
  return (p !=dm_null &&  CCI_ENSURE_MAGIC(p));
}

//===================================================
// cciAllocator implementation
// Standard Allocator for use with external modules
//===================================================

class cciAllocator final: public cciAllocatorBase,
                          public cciIAllocator
{
  public:
    cciAllocator() {}

    CCI_DECL_ISUPPORTS

    CCI_IMETHOD_(void*)  NewObject( size_t n, CCI_STATIC_DESTRUCTOR dtor ) override {
      return cciAllocatorBase::NewObject(n,dtor);
    }

    CCI_IMETHOD_(cci_result) RevokeObject( void* p ,bool discard ) override {
      return cciAllocatorBase::RevokeObject(p,discard);
    }

    CCI_IMETHOD_(dm_uint32)  NumberOfInstances() override {
      return cciAllocatorBase::NumberOfInstances();
    }

    CCI_IMETHOD_(bool) IsObjectValid(void* p) override {
      return cciAllocatorBase::IsObjectValid(p);
    }

  private:
    ~cciAllocator() {}

};

CCI_IMPL_THREADSAFE_ISUPPORTS1(cciAllocator,cciIAllocator);

//===================================================

/**
 * CCI api functions
 */

extern "C" {

__daim_export cci_result
CCI_NewAllocator(cciIAllocator* *result)
{
  cciIAllocator* gc = new cciAllocator;
  if(!gc)
    return CCI_ERROR_OUT_OF_MEMORY;

  CCI_ADDREF(*result = gc);
  return CCI_OK;
}

__daim_export void*
CCI_NewObject(size_t n, CCI_STATIC_DESTRUCTOR _dtor)
{
  CCI_PRECONDITION(gAllocatorState != dm_null,"Allocator not available !");
  if(gAllocatorState)
     return gAllocatorState->mAllocator.NewObject(n,_dtor);

  return dm_null;
}

__daim_export void
CCI_RevokeObject(void* ptr ,bool discard )
{
  CCI_PRECONDITION(gAllocatorState != dm_null,"Allocator not available !");
  if(gAllocatorState)
     gAllocatorState->mAllocator.RevokeObject(ptr,discard);
}

__daim_export cci_result
CCI_VerifyObject(void* ptr)
{
  CCI_PRECONDITION(gAllocatorState != dm_null,"Allocator not available !");
  if(gAllocatorState)
  {    
    if(gAllocatorState->mAllocator.IsObjectValid(ptr))
       return CCI_OK;

    return CCI_ERROR_INVALID_POINTER;
  }
  return CCI_ERROR_UNEXPECTED;
}

}
