#ifndef cciId_h
#define cciId_h

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
 *  David Marteau (CCI implementation)
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

#ifndef cciCore_h
#include "cciCore.h"
#endif

#define DMID_LENGTH 39

/**
 * A "unique identifier". This is modeled after OSF DCE UUIDs.
 * @status FROZEN
 */

struct dmID {
  /**
   * @name Identifier values
   */

  //@{
  dm_uint32 m0;
  dm_uint16 m1;
  dm_uint16 m2;
  dm_uint8  m3[8];
  //@}

  /**
   * @name Methods
   */

  //@{
  /**
   * Equivalency method. Compares this nsID with another.
   * @return <b>PR_TRUE</b> if they are the same, <b>PR_FALSE</b> if not.
   */

  inline bool Equals(const dmID& other) const {
    // One would think that this could be done faster with a really
    // efficient implementation of memcmp(), but evidently no
    // memcmp()'s out there are better than this code.
    //
    // See bug http://bugzilla.mozilla.org/show_bug.cgi?id=164580 for
    // details.

    return
      ((((dm_uint32*) &m0)[0] == ((dm_uint32*) &other.m0)[0]) &&
       (((dm_uint32*) &m0)[1] == ((dm_uint32*) &other.m0)[1]) &&
       (((dm_uint32*) &m0)[2] == ((dm_uint32*) &other.m0)[2]) &&
       (((dm_uint32*) &m0)[3] == ((dm_uint32*) &other.m0)[3]));

  }

  bool Parse(const char *aIDStr);

  //@}
};

/*
 * Class IDs
 */

typedef dmID dmCID;

// Define an CID
#define CCI_DEFINE_CID(_name, _cidspec) \
  const dmCID _name = _cidspec

#define DM_REFCID const dmCID&

/**
 * An "interface id" which can be used to uniquely identify a given
 * interface.
 */

typedef dmID dmIID;

/**
 * A macro shorthand for <tt>const dmIID&<tt>
 */

#define DM_REFIID const dmIID&

/**
 * A macro to build the static const IID accessor method. The Dummy
 * template parameter only exists so that the kIID symbol will be linked
 * properly (weak symbol on linux, gnu_linkonce on mac, multiple-definitions
 * merged on windows). Dummy should always be instantiated as "int".
 */

#define CCI_DECLARE_STATIC_IID_ACCESSOR(the_iid)                       \
  template <class Dummy>                                                \
  struct COMTypeInfo                                                    \
  {                                                                     \
    static const dmIID kIID;                                  \
  };                                                                    \
  static const dmIID& GetIID() {return COMTypeInfo<int>::kIID;}//

#define CCI_DEFINE_STATIC_IID_ACCESSOR(the_interface, the_iid)         \
  template <class Dummy>                                                \
  const dmIID the_interface::COMTypeInfo<Dummy>::kIID = the_iid;//

/**
 * A macro to build the static const CID accessor method
 */

#define CCI_DEFINE_STATIC_CID_ACCESSOR(the_cid) \
  static const dmID& GetCID() {static const dmID cid = the_cid; return cid;}

#define CCI_GET_IID(T) (::T::COMTypeInfo<int>::kIID)
#define CCI_GET_TEMPLATE_IID(T) (T::template COMTypeInfo<int>::kIID)

#endif // CCI_Core_h

