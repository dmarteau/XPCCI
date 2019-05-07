
// Need for dmASSERT definition
#include "dm_utilities.h"
#include <errno.h> // EBUSY defined here
#include <stdio.h>

static  pthread_mutexattr_t  __pthread_mutex_attr;
static  pthread_mutexattr_t* __PTHREAD_ATTR_INITIALIZED = NULL;

#ifdef DM_CONFIG_TARGET_BSD
#define PTHREAD_MUTEX_RECURSIVE_NP PTHREAD_MUTEX_RECURSIVE
#endif

//--------------------------------------------------------
// Implementation of dmMutex using pthread
//--------------------------------------------------------
dmMutex::dmMutex()
{
  if(!__PTHREAD_ATTR_INITIALIZED) {
      __PTHREAD_ATTR_INITIALIZED = &__pthread_mutex_attr;

      int res = pthread_mutexattr_init(&__pthread_mutex_attr);
      if(res==0)
         res = pthread_mutexattr_settype(&__pthread_mutex_attr,PTHREAD_MUTEX_RECURSIVE);

      dmDEBUG_ASSERT( res==0 );
  }

  if(pthread_mutex_init(&_Mutex,&__pthread_mutex_attr)!=0)
     dmDebugBreak(DM_DEBUG_ABORT,dm_null,"failed to initialize mutex",__FILE__,__LINE__);
}
//--------------------------------------------------------
dmMutex::~dmMutex()
{
  int status;
  while((status=pthread_mutex_destroy(&_Mutex))!=0 && status!=EINVAL) {
    dmDEBUG_ASSERT( status==EBUSY );
    Unlock();
  }
  pthread_mutex_destroy(&_Mutex);
}
//--------------------------------------------------------
void dmMutex::Lock()
{
  pthread_mutex_lock(&_Mutex);
}
//--------------------------------------------------------
bool dmMutex::TryLock()
{
  return (pthread_mutex_trylock(&_Mutex)==0);
}
//--------------------------------------------------------
void dmMutex::Unlock()
{
  pthread_mutex_unlock(&_Mutex);
}
//--------------------------------------------------------



