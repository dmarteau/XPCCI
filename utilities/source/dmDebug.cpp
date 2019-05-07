/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is mozilla.org code.
 *
 * The Initial Developer of the Original Code is
 * Netscape Communications Corporation.
 * Portions created by the Initial Developer are Copyright (C) 1998
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   IBM Corp.
 *   Henry Sobotka
 *   Benjamin Smedberg <benjamin@smedbergs.us>
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either of the GNU General Public License Version 2 or later (the "GPL"),
 * or the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#include "dmUtilitiesBase.h"
#include "dmString.h"
#include "dmEnvironment.h"
#include "dmExceptions.h"
#include "dmErrors.h"

#if defined(DM_CONFIG_TARGET_UNIX) || defined(_WIN32)
/* for abort() and getenv() */
  #include <stdlib.h>
#endif

#if defined(DM_CONFIG_TARGET_UNIX)
  #include <signal.h>
#endif

#include <stdio.h>

#include "dmDebug.h"

static void Abort();

#if defined(_WIN32)
  #include <windows.h>
  #include <signal.h>
#elif defined(DM_CONFIG_TARGET_UNIX)
  #include <stdlib.h>
#endif

__dmUtilities
void dmDebugBreak(dm_uint32 aSeverity,
                   const char *aStr,
                   const char *aExpr,
                   const char *aFile, dm_uint32 aLine)
{
   const char *sevString = "WARNING";

   switch (aSeverity) {
   case DM_DEBUG_ASSERTION:
     sevString = "###!!! ASSERTION";
     break;

   case DM_DEBUG_BREAK:
     sevString = "###!!! BREAK";
     break;

   case DM_DEBUG_ABORT:
     sevString = "###!!! ABORT";
     break;

   default:
     aSeverity = DM_DEBUG_WARNING;
   };

   fprintf(stderr,"%s: ",sevString);

   if(aStr)
      fprintf(stderr,"%s: ",aStr);

   if(aExpr)
      fprintf(stderr,"%s: ",aExpr);

   if(aFile)
      fprintf(stderr,"file %s, ",aFile);

   if(aLine != dm_noindex)
     fprintf(stderr,"line %d\n",aLine);

   fflush(stderr);

   switch (aSeverity) {
   case DM_DEBUG_WARNING:
     return;

   case DM_DEBUG_ABORT:
     Abort();
     return;
   }

   // DM_DEBUG_BREAK || DM_DEBUG_ASSERTION
   __dm_DebugBreak();
}


static void Abort()
{
#if defined(_WIN32)
  raise(SIGABRT);  //This should exit us
  _exit(3);        //If we are ignored exit this way..
#elif defined(DM_CONFIG_TARGET_UNIX)
  abort();
#else
  // Don't know how to abort on this platform! call Break() instead
  __dm_DebugBreak();
#endif
}

////////////////////////////////////////////////////////////////////////////////

