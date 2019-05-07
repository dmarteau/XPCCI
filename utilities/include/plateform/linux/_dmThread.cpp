//===============================
// Posix thread specific file
//===============================
#include <pthread.h>

__dmUtilities dm_thread_t dmGetCurrentThreadId()
{
  return pthread_self();
}
