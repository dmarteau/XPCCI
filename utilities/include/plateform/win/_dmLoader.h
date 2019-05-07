//----------------------------------------
// Dynamic loader stubs for win32
//----------------------------------------

// WARNING: on windows LoadLibraryEx() with LOAD_WITH_ALTERED_SEARCH_PATH
// work if the path is *real* window path (i.e with '\'  as separator) , otherwise
// the function fail

#define __dm_dlopen( filename )    ::LoadLibraryEx(filename,NULL,LOAD_WITH_ALTERED_SEARCH_PATH)
#define __dm_dlsym( handle, name ) ::GetProcAddress(handle,name)
#define __dm_dlclose( handle )     ::FreeLibrary( handle )
#ifdef DM_UTILITIES
#define __dm_log_dlerror() \
  dmLOG_ERROR("ld error : %ld\n",::GetLastError())
#else
#define __dm_log_dlerror() \
   fprintf(stderr,"ld error : %ld\n",::GetLastError())
#endif
