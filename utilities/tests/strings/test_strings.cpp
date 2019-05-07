
#include "test_common.h"

int main( int argc, char ** argv )
{
  //__dm_DebugBreak();

  DM_BEGIN_TEST_BLOCK

    const dm_unichar* mstring     = DM_LL("A unicode string");
    const dm_unichar* mfindstring = DM_LL("unicode");

    dmString  aString(mstring);
    dmCString aCString;

    fwprintf(stderr,DM_LL("A unicode string: \"%ls\"\n"),aString.get());

    DM_TEST_EXPECT_RESULT( aString.IsEqual(mstring) );
    DM_TEST_EXPECT_RESULT( aString.Find(mfindstring) == 2 );

    aCString.Assign(mstring);
    fprintf(stderr,"unicode (size: %d) -> ascii (size: %d, value: \"%s\")\n",
            aString.Size(),
            aCString.Size(),
            aCString.get());

    DM_TEST_EXPECT_RESULT( aCString.Size() == aString.Size() );

    aString.Assign(aCString);
    fwprintf(stderr,DM_LL("ascii (size: %d) -> unicode (size: %d, value: \"%ls\")\n"),
            aCString.Size(),
            aString.Size(),
            aString.get());

    DM_TEST_EXPECT_RESULT( aString.Size() == aCString.Size() );

  DM_END_TEST_BLOCK

  DM_EXIT_TEST
}
