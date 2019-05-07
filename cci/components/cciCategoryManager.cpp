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
 * Portions created by the Initial Developer are Copyright (C) 2000
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Scott Collins <scc@netscape.com>
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

#include "cciPrivate.h"
#include "cciCategoryManager.h"
#include "cciIStringEnumerator.h"

#define CATLEAF_BLOCKS 64
#define CATNODE_BLOCKS 32

//
// BaseStringEnumerator is subclassed by EntryEnumerator and
// CategoryEnumerator
//
class BaseStringEnumerator final : public cciIUTF8StringEnumerator,
                                   public cciIStringEnumerator
                             
{
public:
  CCI_DECL_ISUPPORTS
  CCI_IMETHOD GetNext(dmAString& aResult) override;
  CCI_DECL_IUTF8STRINGENUMERATOR

BaseStringEnumerator(dmHashPtrDict::iterator first,dmHashPtrDict::iterator end,
                     cciISupports* owner, dmRWLock& aLock)
    : mCurrent(first)
    , mEnd(end)
    , mOwner(owner)
    , mRLock(aLock)
    {}

  dmHashPtrDict::iterator mCurrent;
  dmHashPtrDict::iterator mEnd;

  cci_Ptr<cciISupports> mOwner;
  dmReadLock            mRLock;

private:
  ~BaseStringEnumerator() {}
};

CCI_IMPL_ISUPPORTS1(BaseStringEnumerator, cciIStringEnumerator)

CCI_IMETHODIMP_(bool)
BaseStringEnumerator::HasMore()
{
  return mCurrent==mEnd ? false : true;
}

CCI_IMETHODIMP
BaseStringEnumerator::GetNext(dmACString& _retval)
{
  if(mCurrent==mEnd)
     return CCI_ERROR_FAILURE;

  _retval.Assign((*mCurrent++).first);
  return CCI_OK;
}

CCI_IMETHODIMP
BaseStringEnumerator::GetNext(dmAString& _retval)
{
  if(mCurrent==mEnd)
     return CCI_ERROR_FAILURE;

  _retval.Assign((*mCurrent++).first);
  return CCI_OK;
}

//
// CategoryLeaf implementation
//

class cciCategoryLeaf
{
public:
  ~cciCategoryLeaf() {}
   cciCategoryLeaf( const char* value, bool persists )
   {
     mValue = value;
     if(persists)
        mPersistentValue = mValue;
   }

  void SetValue(const char* value,bool persists)
  {
    mValue =  value;
    if(persists)
       mPersistentValue = mValue;
  }

  dmCString mPersistentValue;
  dmCString mValue;

  DM_DECL_ALLOCATOR_NEW(cciCategoryLeaf,CATLEAF_BLOCKS)
};

DM_IMPL_ALLOCATOR_NEW(cciCategoryLeaf,true);

//
// CategoryNode implementations
//

class cciCategoryNode
{
public:
  cciCategoryNode() {}
 ~cciCategoryNode() {
    Clear();
  }

  dmHashPtrDict mEntries;
  dmRWLock      mLock;

  cci_result Enumerate(cciISupports*, cciIUTF8StringEnumerator **_retval);
  cci_result AddEntry( const char* aEntry, const char* aValue,
                       bool persist, bool replace);

  cci_result RemoveEntry(const char* aEntry, bool persist);

  bool WritePersistentEntries(FILE* fd, const char* aCategoryName);

  cciCategoryLeaf* GetEntry( const dmCString& aEntry )
  {
    dmHashPtrDict::iterator it = mEntries.find(aEntry);
    if(it!=mEntries.end())
       return reinterpret_cast<cciCategoryLeaf*>((*it).second);

    return dm_null;
  }

  void Clear();

  DM_DECL_ALLOCATOR_NEW(cciCategoryNode,CATNODE_BLOCKS)
};

DM_IMPL_ALLOCATOR_NEW(cciCategoryNode,true);

cci_result cciCategoryNode::Enumerate(cciISupports* aOwner, cciIUTF8StringEnumerator **_retval)
{
  CCI_ENSURE_ARG_POINTER(_retval);

  cciIUTF8StringEnumerator* enumerator =
    new BaseStringEnumerator(mEntries.begin(),mEntries.end(),aOwner,mLock);

  CCI_ADDREF(*_retval = enumerator);
  return CCI_OK;
}

cci_result cciCategoryNode::AddEntry( const char* aEntry, const char* aValue,
                                      bool persist, bool replace)
{
  dmWRITE_LOCK(mLock);

  cciCategoryLeaf* leaf = GetEntry(dmCString(aEntry));
  if(leaf)
  {
    if(replace)
       leaf->SetValue(aValue,persist);
  }
  else
    mEntries[dmCString(aEntry)] = new cciCategoryLeaf(aValue,persist);

  return CCI_OK;
}

cci_result cciCategoryNode::RemoveEntry( const char* aEntry, bool persist )
{
  dmWRITE_LOCK(mLock);

  dmCString entry(aEntry);

  cciCategoryLeaf* leaf = GetEntry(entry);
  if(leaf)
  {
    leaf->mValue.Clear();
    if(persist) {
      mEntries.erase(entry);
      delete leaf;
    }
  }
  return CCI_OK;
}

void cciCategoryNode::Clear()
{
  dmWRITE_LOCK(mLock);

  // Clear factory entries
  dmHashPtrDict::iterator it  = mEntries.begin();
  dmHashPtrDict::iterator end = mEntries.end();
  while(it!=end)
  {
    // Factory destructor (if any) will be called here
    cciCategoryLeaf* leaf = reinterpret_cast<cciCategoryLeaf*>((*it++).second);
    if(leaf)
       delete leaf;
  }

  mEntries.clear();
}

bool cciCategoryNode::WritePersistentEntries(FILE* fd, const char* aCategoryName)
{
  dmREAD_LOCK(mLock);

  dmHashPtrDict::iterator it  = mEntries.begin();
  dmHashPtrDict::iterator end = mEntries.end();
  while(it!=end)
  {
    cciCategoryLeaf* leaf = reinterpret_cast<cciCategoryLeaf*>((*it).second);
    if(!leaf->mPersistentValue.IsEmpty())
    {
      if(fprintf(fd,"%s,%s,%s\n",aCategoryName,(*it).first.get(),
                 leaf->mPersistentValue.get()) ==  -1)
        return false;
    }

    ++it;
  }
  return true;
}

//
// cciCategoryManager implementations
//

CCI_IMPL_THREADSAFE_ISUPPORTS1(cciCategoryManager, cciICategoryManager)

cciCategoryManager*
cciCategoryManager::Create()
{
  cciCategoryManager* manager = new cciCategoryManager();

  return manager;
}

cciCategoryManager::~cciCategoryManager()
{
  Clear();
}

void cciCategoryManager::Clear()
{
  // Clear factory entries
  dmHashPtrDict::iterator it  = mCategories.begin();
  dmHashPtrDict::iterator end = mCategories.end();
  while(it!=end)
  {
    // Factory destructor (if any) will be called here
    cciCategoryNode* node = reinterpret_cast<cciCategoryNode*>((*it++).second);
    if(node)
       delete node;
  }

  mCategories.clear();
}


cciCategoryNode* cciCategoryManager::get_category(const dmCString& aCategory)
{
  {
    dmHashPtrDict::iterator it = mCategories.find(aCategory);
    if(it!=mCategories.end())
       return reinterpret_cast<cciCategoryNode*>((*it).second);
  }

  return dm_null;
}

CCI_IMETHODIMP
cciCategoryManager::GetCategoryEntry( const char *aCategoryName,
                                      const char *aEntryName,
                                      dmACString& _retval )
{
  CCI_ENSURE_ARG_POINTER(aCategoryName);
  CCI_ENSURE_ARG_POINTER(aEntryName);

  cci_result rv = CCI_ERROR_NOT_AVAILABLE;

  dmREAD_LOCK(mLock);

  cciCategoryNode* category = get_category(dmCString(aCategoryName));
  if(category)
  {
    cciCategoryLeaf* leaf = category->GetEntry(dmCString(aEntryName));
    if(leaf) {
      _retval = leaf->mValue;
      rv = CCI_OK;
    }
  }

  return rv;
}

CCI_IMETHODIMP
cciCategoryManager::AddCategoryEntry( const char *aCategoryName,
                                      const char *aEntryName,
                                      const char *aValue,
                                      bool aPersist,
                                      bool aReplace)
{
  CCI_ENSURE_ARG_POINTER(aCategoryName);
  CCI_ENSURE_ARG_POINTER(aEntryName);
  CCI_ENSURE_ARG_POINTER(aValue);

  // Before we can insert a new entry, we'll need to
  //  find the |CategoryNode| to put it in...

  mLock.WrLock();

  cciCategoryNode* category = get_category(dmCString(aCategoryName));

  if (!category)
  {
    category = new cciCategoryNode;
    mCategories[dmCString(aCategoryName)] = category;
  }
  mLock.Unlock();

  if(!category)
    return CCI_ERROR_OUT_OF_MEMORY;

  return category->AddEntry(aEntryName,aValue,aPersist,aReplace);
}

CCI_IMETHODIMP
cciCategoryManager::DeleteCategoryEntry( const char *aCategoryName,
                                         const char *aEntryName,
                                         bool aPersist)
{
  CCI_ENSURE_ARG_POINTER(aCategoryName);
  CCI_ENSURE_ARG_POINTER(aEntryName);

  dmWRITE_LOCK(mLock);

  cciCategoryNode* category = get_category(dmCString(aCategoryName));
  if(category)
     category->RemoveEntry(aEntryName,aPersist);

  // Return OK because we don't care if it doesn't exists
  return CCI_OK;
}

CCI_IMETHODIMP
cciCategoryManager::DeleteCategory( const char *aCategoryName )
{
  CCI_ENSURE_ARG_POINTER(aCategoryName);

  dmWRITE_LOCK(mLock);

  dmCString catName(aCategoryName);

  cciCategoryNode* category = get_category(catName);
  if(category)
  {
     // Prevent acces if the category is in use
     dmWRITE_LOCK(category->mLock);
     delete category;
     mCategories.erase(catName);
  }
  // Return OK because we don't care if it doesn't exists
  return CCI_OK;
}

CCI_IMETHODIMP
cciCategoryManager::EnumerateCategory( const char *aCategoryName,
                                       cciIUTF8StringEnumerator **_retval )
{
  CCI_ENSURE_ARG_POINTER(aCategoryName);
  CCI_ENSURE_ARG_POINTER(_retval);

  dmREAD_LOCK(mLock);

  cciCategoryNode* category = get_category(dmCString(aCategoryName));
  if(category)
     return category->Enumerate(CCI_ISUPPORTS_CAST(cciICategoryManager*,this),_retval);

  return CCI_ERROR_NOT_AVAILABLE;
}

CCI_IMETHODIMP
cciCategoryManager::EnumerateCategories(cciIUTF8StringEnumerator **_retval)
{
  CCI_ENSURE_ARG_POINTER(_retval);

  dmREAD_LOCK(mLock);

  cciIUTF8StringEnumerator* enumerator =
    new BaseStringEnumerator(mCategories.begin(),mCategories.end(),
                             CCI_ISUPPORTS_CAST(cciICategoryManager*,this),mLock);

  CCI_ADDREF(*_retval = enumerator);
  return CCI_OK;

}

CCI_METHOD
cciCategoryManager::WriteCategoryManagerToRegistry(FILE* fd)
{
  // Clear factory entries
  dmHashPtrDict::iterator it  = mCategories.begin();
  dmHashPtrDict::iterator end = mCategories.end();

  while(it!=end)
  {
    // Factory destructor (if any) will be called here
    cciCategoryNode* node = reinterpret_cast<cciCategoryNode*>((*it).second);
    if(node) {
       cci_result rv = node->WritePersistentEntries(fd,(*it).first.get());
       if(CCI_FAILED(rv))
          return rv;
    }

    ++it;
  }

  return CCI_OK;
}

//////////////////////////////////////////////////////
// cciCategoryManagerFactory
//

class cciCategoryManagerFactory final : public cciIFactory
   {
     public:
       cciCategoryManagerFactory() { }

       CCI_DECL_ISUPPORTS
       CCI_DECL_IFACTORY
   };

CCI_IMPL_ISUPPORTS1(cciCategoryManagerFactory, cciIFactory)

CCI_IMETHODIMP
cciCategoryManagerFactory::CreateInstance( cciISupports* aOuter, const dmIID& aIID, void** aResult )
{
  CCI_ENSURE_ARG_POINTER(aResult);

  *aResult = 0;

  cci_result status = CCI_OK;
  if ( aOuter )
    status = CCI_ERROR_NO_AGGREGATION;
  else
    {
      cciCategoryManager* raw_category_manager = cciCategoryManager::Create();
      cci_Ptr<cciICategoryManager> new_category_manager = raw_category_manager;
      if(new_category_manager)
        status = new_category_manager->QueryInterface(aIID, aResult);
      else
        status = CCI_ERROR_OUT_OF_MEMORY;
    }

  return status;
}

CCI_IMETHODIMP
cciCategoryManagerFactory::LockFactory( bool )
{
    // Not implemented...
  return CCI_OK;
}

cci_result
CCI_CategoryManagerGetFactory( cciIFactory** aFactory )
{
  // assert(aFactory);

  cci_result status;

  *aFactory = 0;
  cciIFactory* new_factory = static_cast<cciIFactory*>(new cciCategoryManagerFactory);
  if (new_factory)
    {
      *aFactory = new_factory;
      CCI_ADDREF(*aFactory);
      status = CCI_OK;
    }
  else
    status = CCI_ERROR_OUT_OF_MEMORY;

  return status;
}

