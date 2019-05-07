
#ifndef cciAllocatorImpl_h
#define cciAllocatorImpl_h

#ifndef cciIAllocator_h
#include "cciIAllocator.h"
#endif


/**
 * Allocator implementation
 */
class cciAllocatorBase  {
public:
  cciAllocatorBase();

  void*       NewObject( size_t n, CCI_STATIC_DESTRUCTOR _dtor );
  cci_result  RevokeObject( void* p ,bool discard );

  dm_uint32   NumberOfInstances();
  bool     IsObjectValid(void* p);

protected:
  dmList    mInstances;
  dm_uint32 mNuminsts;
  dmMutex   mLock;

  void FreeAllObjects();

public:
  static cci_result InitAllocatorState();
  static void       ReleaseAllocatorState();
  static void       FreeGarbageList();

  ~cciAllocatorBase();
};


#endif // cciAllocatorImpl_h

