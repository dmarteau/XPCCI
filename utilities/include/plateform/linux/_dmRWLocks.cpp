//--------------------------------------------------------
// Implementation of read/write pthreads locks
//--------------------------------------------------------
dmRWLock::dmRWLock()
{
   pthread_rwlock_init(&_thisLock,0);
}
//--------------------------------------------------------
dmRWLock::~dmRWLock() 
{
  while(pthread_rwlock_destroy(&_thisLock)!=0)
    Unlock();

  pthread_rwlock_destroy(&_thisLock);
}
//--------------------------------------------------------
void dmRWLock::RdLock()
{
   pthread_rwlock_rdlock(&_thisLock);
}
//--------------------------------------------------------
bool dmRWLock::TryRdLock() 
{
  return (pthread_rwlock_tryrdlock(&_thisLock)==0);
}
//--------------------------------------------------------
void dmRWLock::WrLock()
{
  pthread_rwlock_wrlock(&_thisLock);
}
//--------------------------------------------------------
bool dmRWLock::TryWrLock() 
{
  return (pthread_rwlock_trywrlock(&_thisLock)==0);
}
//--------------------------------------------------------
void dmRWLock::Unlock() 
{
  pthread_rwlock_unlock(&_thisLock);
}
//--------------------------------------------------------
