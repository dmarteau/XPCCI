
#if defined(DM_UTILITIES)
#include "dm_utilities.h"
#elif defined(XPCCI_LIB)
#include "xpcci.h"
#elif defined(XPCCI_GLUE)
#include "xpcciglue.h"
#endif

#include "dmCrt.h"

static bool gFailed = false;
static const char* mArgv[] = {
  "--enable-trace-memory",
  #if defined(XPCCI_LIB) && defined(CCI_REGISTRY_LOCATION)
  "--registry-location", CCI_REGISTRY_LOCATION,
  #endif
};

#if defined(DM_UTILITIES)

#define DM_BEGIN_TEST_BLOCK \
  dmUtilities::Initialize(mArgv,sizeof(mArgv)/sizeof(char*)); \
  dmTRY

#define DM_END_TEST_BLOCK      \
  dmCATCH(_E)                  \
    dmTRACE_EXCEPTION(_E);     \
    gFailed = true;         \
  dmDONE                       \
  end_test:                    \
  dmUtilities::Finalize();

#elif defined(XPCCI_LIB)

#define DM_BEGIN_TEST_BLOCK \
cci_result rv = DM_Initialize(mArgv,sizeof(mArgv)/sizeof(char*),0); \
if(CCI_SUCCEEDED(rv)) {

#define DM_END_TEST_BLOCK    \
  end_test:                  \
  DM_Finalize(false);        \
} else {                     \
  fprintf(stderr,"urggh, failed to initialize DAIM library with error %d.\n",rv); \
  gFailed = true;         \
}

#elif defined(XPCCI_GLUE)

#define DM_BEGIN_TEST_BLOCK \
cci_result rv = DM_InitDaimGlue(mArgv,sizeof(mArgv)/sizeof(char*),0); \
if(CCI_SUCCEEDED(rv)) {

#define DM_END_TEST_BLOCK    \
  end_test:                  \
  DM_ReleaseDaimGlue(false); \
} else {                     \
  fprintf(stderr,"urggh, failed to initialize DAIM library with error %d.\n",rv); \
  gFailed = true;         \
}


#endif //!XPCCI_LIB

#define DM_EXIT_TEST \
  fprintf(stderr,"%s\n",gFailed?"******* TEST FAILED":"******* TEST PASSED"); \
  return gFailed?2:0;

#define DM_TEST_EXPECT_RESULT(_expr)                                  \
  DM_BEGIN_MACRO                                                      \
    fprintf(stderr,"%s...",#_expr);                                   \
    if (!(_expr)) {                                                   \
      fprintf(stderr,"failed at %s , line %d\n",__FILE__, __LINE__);    \
      gFailed = true;                                              \
      goto end_test;                                                  \
    } else                                                            \
      fprintf(stderr,"passed\n");                                     \
  DM_END_MACRO
