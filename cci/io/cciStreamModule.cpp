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
// File         : cciStreamModule.cpp
// Date         : 27 nov. 2008
// Author       : David Marteau
//--------------------------------------------------------

#include "cciString.h"
#include "cciServiceManagerUtils.h"
#include "cciIComponentManager.h"
#include "cciIGenericFactory.h"

#include "cciFileStreams.h"
#include "cciMemoryInputStream.h"

CCI_GENERIC_FACTORY_CONSTRUCTOR(cciFileInputStream)
CCI_GENERIC_FACTORY_CONSTRUCTOR(cciFileOutputStream)
CCI_GENERIC_FACTORY_CONSTRUCTOR(cciMemoryInputStream)

static const cciModuleComponentInfo components[] = {
    { CCI_FILEINPUTSTREAM_CLASSNAME,
      CCI_FILEINPUTSTREAM_CID,
      CCI_FILEINPUTSTREAM_CONTRACTID,
      cciFileInputStreamConstructor
    },
    { CCI_FILEOUTPUTSTREAM_CLASSNAME,
      CCI_FILEOUTPUTSTREAM_CID,
      CCI_FILEOUTPUTSTREAM_CONTRACTID,
      cciFileOutputStreamConstructor
    },
    { CCI_MEMORYINPUTSTREAM_CLASSNAME,
      CCI_MEMORYINPUTSTREAM_CID,
      CCI_MEMORYINPUTSTREAM_CONTRACTID,
      cciMemoryInputStreamConstructor
    },
};

CCI_IMPL_GETMODULE(StreamModule, components)

