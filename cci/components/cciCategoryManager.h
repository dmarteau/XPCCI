#ifndef cciCategoryManager_h
#define cciCategoryManager_h
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

#ifndef cciCOM_h
#include "cciCOM.h"
#endif

#ifndef cciCOMPtr_h
#include "cciCOMPtr.h"
#endif

#ifndef cciIFactory_h
#include "cciIFactory.h"
#endif

#include <stdio.h>

#include "cciICategoryManager.h"

#define CCI_CATEGORYMANAGER_CLASSNAME     "Category Manager"
#define CCI_CATEGORYMANAGER_CONTRACTID    "@daim.org/category-manager;1"

/* 16d222a6-1dd2-11b2-b693-f38b02c021b2 */
#define CCI_CATEGORYMANAGER_CID \
{ 0x16d222a6, 0x1dd2, 0x11b2, \
  {0xb6, 0x93, 0xf3, 0x8b, 0x02, 0xc0, 0x21, 0xb2} }

/**
 * The main implementation of nsICategoryManager.
 *
 * This implementation is thread-safe.
 */

class cciCategoryNode;

class cciCategoryManager final : public cciICategoryManager
{
public:
  CCI_DECL_ISUPPORTS
  CCI_DECL_ICATEGORYMANAGER

  /**
   * Write the categories to the XPCOM persistent registry.
   * This is to be used by cciComponentManagerImpl (and NO ONE ELSE).
   */
  CCI_METHOD WriteCategoryManagerToRegistry(FILE* fd);

  cciCategoryManager() {}
private:
  friend class cciCategoryManagerFactory;
  static cciCategoryManager* Create();

  ~cciCategoryManager();

  void Clear();

  cciCategoryNode* get_category(const dmCString& aCategory);

  dmHashPtrDict mCategories;
  dmRWLock      mLock;
};

cci_result
CCI_CategoryManagerGetFactory( cciIFactory** aFactory );

#endif // cciCategoryManager_h
