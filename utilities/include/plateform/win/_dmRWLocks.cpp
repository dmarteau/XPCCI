//--------------------------------------------------------
// Implementation of read/write locks for Win32
//--------------------------------------------------------
dmRWLock::dmRWLock()
{
  ::InitializeCriticalSection(&_ExclusiveMutex);

  // Create Semaphore in open state
  _SharedAccessCompleted = ::CreateSemaphore(NULL,1,1,NULL);
  _SharedAccessCount     = 0;
}
//--------------------------------------------------------
dmRWLock::~dmRWLock() 
{
  ::DeleteCriticalSection(&_ExclusiveMutex);
  ::CloseHandle(_SharedAccessCompleted);
}
//--------------------------------------------------------
void dmRWLock::RdLock()
{
  ::EnterCriticalSection(&_ExclusiveMutex);

  // Semaphore count for shared acess
  if(++_SharedAccessCount == 1) 
    ::WaitForSingleObject(_SharedAccessCompleted,INFINITE); // S -> 0
    
  ::LeaveCriticalSection(&_ExclusiveMutex);
}
//--------------------------------------------------------
bool dmRWLock::TryRdLock() 
{
  if(::TryEnterCriticalSection(&_ExclusiveMutex)==TRUE)
  {
    // Semaphore count for shared acess
    if(++_SharedAccessCount == 1) 
     ::WaitForSingleObject(_SharedAccessCompleted,INFINITE);
    
    ::LeaveCriticalSection(&_ExclusiveMutex);
    return true; 
  }
  return false;
}
//--------------------------------------------------------
void dmRWLock::WrLock()
{
  ::EnterCriticalSection(&_ExclusiveMutex);

  if(_SharedAccessCount > 0) {
    ::WaitForSingleObject(_SharedAccessCompleted,INFINITE); // Wait for readers to complete
    ::ReleaseSemaphore   (_SharedAccessCompleted,1,NULL);   // Restore semaphore
  }
}
//--------------------------------------------------------
bool dmRWLock::TryWrLock() 
{
  if(::TryEnterCriticalSection(&_ExclusiveMutex)==TRUE)
  {
     if(_SharedAccessCount <= 0)
        return true; 

     ::LeaveCriticalSection(&_ExclusiveMutex);
  }
  return false;
}
//--------------------------------------------------------
void dmRWLock::Unlock() 
{
  //------------------------------------------------------
  // A read lock never enter when _SharedAccessCount > 0
  // and a write lock cannot enter when a read lock is active
  // that mean if _SharedAccessCount > we unlock a read lock.
  //------------------------------------------------------
  if(_SharedAccessCount > 0)
  {
    ::EnterCriticalSection(&_ExclusiveMutex);

    if(--_SharedAccessCount==0)
      ::ReleaseSemaphore(_SharedAccessCompleted,1,NULL); // Restore semaphore

    ::LeaveCriticalSection(&_ExclusiveMutex);
  }
  else
    ::LeaveCriticalSection(&_ExclusiveMutex);
}
//--------------------------------------------------------
