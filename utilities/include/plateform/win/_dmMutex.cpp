//--------------------------------------------------------
// Implementation of dmMutex for Win32
// Using critical section for performances
//--------------------------------------------------------
dmMutex::dmMutex()
{
  ::InitializeCriticalSection(&_Mutex);
}
//--------------------------------------------------------
dmMutex::~dmMutex() 
{
  ::DeleteCriticalSection(&_Mutex);
}
//--------------------------------------------------------
void dmMutex::Lock()
{
  ::EnterCriticalSection(&_Mutex);
}
//--------------------------------------------------------
bool dmMutex::TryLock() 
{
  return ::TryEnterCriticalSection(&_Mutex)==TRUE;
}
//--------------------------------------------------------
void dmMutex::Unlock() 
{
  ::LeaveCriticalSection(&_Mutex);
}
//--------------------------------------------------------



