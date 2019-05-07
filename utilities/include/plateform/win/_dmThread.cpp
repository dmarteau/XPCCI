//===============================
// Win32 specific file
//===============================
__dmUtilities dm_thread_t dmGetCurrentThreadId()
{
  return ::GetCurrentThreadId();
}
