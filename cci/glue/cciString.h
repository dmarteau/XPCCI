/* ::: BEGIN LICENSE BLOCK:::
 * Copyright (c) 2005 David Marteau
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
 *  ::: END LICENSE BLOCK::: */

#ifndef cciString_h
#define cciString_h

#ifndef cciCore_h
#include "cciCore.h"
#endif

class dmACString;
class dmAString;

class cciStringContainer;
class cciCStringContainer;

extern "C" {

__daim_export cci_result  __daim_call( CCI_CStringContainerInit   )(cciCStringContainer &aContainer);
__daim_export void        __daim_call( CCI_CStringContainerFinish )(cciCStringContainer &aContainer);
__daim_export cci_result  __daim_call( CCI_CStringSetData   )(dmACString &aStr, const char  *aData, dm_uint32 aDataLength);
__daim_export dm_uint32   __daim_call( CCI_CStringGetData   )(const dmACString &aStr, const char **aData );
__daim_export cci_result  __daim_call( CCI_CStringAppendData)(dmACString &aStr, const char  *aData, dm_uint32 aDataLength );
__daim_export cci_result  __daim_call( CCI_CStringCopy      )(dmACString &aDestStr, const dmACString& aSrcStr );

__daim_export cci_result  __daim_call( CCI_StringContainerInit   )(cciStringContainer &aContainer);
__daim_export void        __daim_call( CCI_StringContainerFinish )(cciStringContainer &aContainer);
__daim_export cci_result  __daim_call( CCI_StringSetData   )(dmAString &aStr, const dm_unichar  *aData, dm_uint32 aDataLength);
__daim_export dm_uint32   __daim_call( CCI_StringGetData   )(const dmAString &aStr, const dm_unichar **aData );
__daim_export cci_result  __daim_call( CCI_StringAppendData)(dmAString &aStr, const dm_unichar *aData, dm_uint32 aDataLength );
__daim_export cci_result  __daim_call( CCI_StringCopy      )(dmAString &aDestStr, const dmAString& aSrcStr );

}

struct cciStringContainer_base {
  void* d1;
};

#ifndef DM_UTILITIES

/*
 * Define a minimal (abstract) interface to deal with
 * dmAString references in cci interface when not using core utilities library
 */

class dmACString
{
  public:
   typedef char        char_type;
   typedef dmACString  self_type;
   typedef dm_uint32   size_type;
   typedef dm_uint32   index_type;

   enum { npos = DM_UINT32_MAX };

   const char_type* get() const
   {
     const char_type* data = dm_null;
     CCI_CStringGetData(*this,&data);
     return data;
   }

   dmACString& Assign(const char_type *s , size_type n = npos) {
     CCI_CStringSetData(*this,s,n);
     return *this;
   }

   dmACString& Assign(const self_type& str) {
     CCI_CStringCopy(*this,str);
     return *this;
   }

   dmACString& Assign(char_type aChar) {
     CCI_CStringSetData(*this,&aChar,1);
     return *this;
   }

   dmACString& Append(const char_type *s , size_type n = npos) {
     CCI_CStringAppendData(*this,s,n);
     return *this;
   }

   self_type& operator=(const self_type& aString) { Assign(aString);  return *this; }
   self_type& operator=(const char_type* aPtr)    { Assign(aPtr);     return *this; }
   self_type& operator=(char_type aChar)          { Assign(aChar);    return *this; }

   size_type Size()    const { return CCI_CStringGetData(*this,dm_null); }
   bool      IsEmpty() const { return Size()==0; }


  protected:
    ~dmACString() {}; // prevent creation
};


class dmAString
{
  public:
   typedef dm_unichar  char_type;
   typedef dmAString   self_type;
   typedef dm_uint32   size_type;
   typedef dm_uint32   index_type;

   enum { npos = DM_UINT32_MAX };

   const char_type* get() const
   {
     const char_type* data = dm_null;
     CCI_StringGetData(*this,&data);
     return data;
   }

   dmAString& Assign(const char_type *s , size_type n = npos) {
     CCI_StringSetData(*this,s,n);
     return *this;
   }

   dmAString& Assign(const self_type& str) {
     CCI_StringCopy(*this,str);
     return *this;
   }

   dmAString& Assign(char_type aChar) {
     CCI_StringSetData(*this,&aChar,1);
     return *this;
   }

   dmAString& Append(const char_type *s , size_type n = npos) {
     CCI_StringAppendData(*this,s,n);
     return *this;
   }

   self_type& operator=(const self_type& aString) { Assign(aString);  return *this; }
   self_type& operator=(const char_type* aPtr)    { Assign(aPtr);     return *this; }
   self_type& operator=(char_type aChar)          { Assign(aChar);    return *this; }

   size_type Size()    const { return CCI_StringGetData(*this,dm_null); }
   bool      IsEmpty() const { return Size()==0; }

  protected:
    ~dmAString() {}; // prevent creation
};


/* ------------------------------------------------------------------------- */

/**
 * Below we define cciStringContainer and cciCStringContainer.  These classes
 * have unspecified structure.  In most cases, your code should use
 * dmString/dmCString instead of these classes; if you prefer C-style
 * programming, then look no further.
 */

class cciStringContainer : public dmAString,
                           private cciStringContainer_base
{
};

class cciCStringContainer : public dmACString,
                            private cciStringContainer_base
{
};

/*
 * Define instantiable class to deal with
 * dmAString references in cci interface when not using core utilities library
 */


class dmCString : public cciCStringContainer
{
  public:
    typedef dmCString  self_type;
    typedef dmACString abstract_string_type;

    dmCString()
    {
      CCI_CStringContainerInit(*this);
    }

    dmCString(const self_type& aString)
    {
      CCI_CStringContainerInit(*this);
      CCI_CStringCopy(*this, aString);
    }

    explicit
    dmCString(const abstract_string_type& aReadable)
    {
      CCI_CStringContainerInit(*this);
      CCI_CStringCopy(*this, aReadable);
    }

    explicit
    dmCString(const char_type* aData, size_type aLength = npos)
    {
      CCI_CStringContainerInit(*this);
      CCI_CStringSetData(*this,aData,aLength);
    }

   ~dmCString()
    {
      CCI_CStringContainerFinish(*this);
    }

   self_type& operator=(const self_type& aString)              { Assign(aString);    return *this; }
   self_type& operator=(const abstract_string_type& aReadable) { Assign(aReadable);  return *this; }
   self_type& operator=(const char_type* aPtr)                 { Assign(aPtr);       return *this; }
   self_type& operator=(char_type aChar)                       { Assign(aChar);      return *this; }
};

class dmString : public cciStringContainer
{
  public:
    typedef dmString  self_type;
    typedef dmAString abstract_string_type;

    dmString()
    {
      CCI_StringContainerInit(*this);
    }

    dmString(const self_type& aString)
    {
      CCI_StringContainerInit(*this);
      CCI_StringCopy(*this, aString);
    }

    explicit
    dmString(const abstract_string_type& aReadable)
    {
      CCI_StringContainerInit(*this);
      CCI_StringCopy(*this, aReadable);
    }

    explicit
    dmString(const char_type* aData, size_type aLength = npos)
    {
      CCI_StringContainerInit(*this);
      CCI_StringSetData(*this,aData,aLength);
    }

   ~dmString()
    {
      CCI_StringContainerFinish(*this);
    }

   self_type& operator=(const self_type& aString)              { Assign(aString);    return *this; }
   self_type& operator=(const abstract_string_type& aReadable) { Assign(aReadable);  return *this; }
   self_type& operator=(const char_type* aPtr)                 { Assign(aPtr);       return *this; }
   self_type& operator=(char_type aChar)                       { Assign(aChar);      return *this; }
};

#else // DM_UTILITIES defined

#ifndef dm_utilities_h
#include "dm_utilities.h"
#endif

#endif // DM_UTILITIES

#endif // cciString_h
