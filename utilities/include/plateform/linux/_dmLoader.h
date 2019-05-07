//----------------------------------------
// Dynamic loader stubs for linux
//----------------------------------------

#include <dlfcn.h>

#define __dm_dlopen( filename )    dlopen(filename,RTLD_LAZY)
#define __dm_dlsym( handle, name ) dlsym(handle,name)
#define __dm_dlclose( handle )     dlclose(handle)

#ifdef DM_UTILITIES
#define __dm_log_dlerror() \
   dmLOG_ERROR("ld error : %s\n",dlerror())
#else
#define __dm_log_dlerror() \
   fprintf(stderr,"ld error : %s\n",dlerror())
#endif

