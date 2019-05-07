/* :::BEGIN LICENSE BLOCK:::
 *
 * Copyright (c) 2004-2005 David Marteau
 *
 * This file is part of the DAIM Image Processing Library (DAIM library)
 *
 * The DAIM library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License (LGPL)
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * The DAIM library is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with the DAIM library; see the file LICENSE.  If
 * not, write to the Free Software Foundation, Inc., 59 Temple Place -
 * Suite 330, Boston, MA 02111-1307, USA.
 *
 *  :::END LICENSE BLOCK::: */

//--------------------------------------------------------
// File         : test_glue.cpp
// Date         : 7 janv. 2009
// Author       : David Marteau
//--------------------------------------------------------

#include "test_common.h"

#include "cciCOMPtr.h"
#include "cciComponentManagerUtils.h"
#include "cciServiceManagerUtils.h"
#include "cciIComponentRegistrar.h"
#include "cciIFileStreams.h"

//==============================
// Very minimal test to check
// daim initialization
//==============================
int main( int argc, char ** argv )
{
  // Get the XPCCI_BIN environment variable
  const char* location = getenv("XPCCI_BIN");

  cci_result rv;

  DM_TEST_EXPECT_RESULT(location && location[0]!='\0');

  printf("\n### Test  1: Can I Init DAIM library ?\n");
  rv = DM_InitXPCCIGlue(location,mArgv,sizeof(mArgv)/sizeof(char*),0);

  DM_TEST_EXPECT_RESULT(CCI_SUCCEEDED(rv));

  {
    printf("\n### Test  2: Can I create cciIFileInputStream instance ?\n");
    cci_Ptr<cciIFileInputStream> fileStream = do_CreateInstance("@daim.org/contrib/file-input-stream;1",&rv);

    DM_TEST_EXPECT_RESULT(CCI_SUCCEEDED(rv));
  }

  end_test:
   DM_ReleaseXPCCIGlue(false);

  DM_EXIT_TEST
}


