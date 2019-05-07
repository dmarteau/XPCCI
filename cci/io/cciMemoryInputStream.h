#ifndef cciBufferStream_h
#define cciBufferStream_h
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
// File         : cciBufferStream.h
// Date         : 26 nov. 2008
// Author       : David Marteau
//--------------------------------------------------------

#include "cciIMemoryInputStream.h"

#define CCI_MEMORYINPUTSTREAM_CID \
{ /* 1E30DE66-7748-4A5C-AEFE-1142864FE9B3 */       \
  0x1E30DE66,                                      \
  0x7748,                                          \
  0x4A5C,                                          \
  {0xAE, 0xFE, 0x11, 0x42, 0x86, 0x4F, 0xE9, 0xB3} \
}

#define CCI_MEMORYINPUTSTREAM_CONTRACTID "@daim.org/contrib/memory-input-stream;1"
#define CCI_MEMORYINPUTSTREAM_CLASSNAME  "In-memory input stream"

/* Header file */
class cciMemoryInputStream final : public cciIMemoryInputStream,
                                   public cciISeekableStream
{
public:
  CCI_DECL_ISUPPORTS
  CCI_DECL_IINPUTSTREAM
  CCI_DECL_IMEMORYINPUTSTREAM
  CCI_DECL_ISEEKABLESTREAM

  cciMemoryInputStream();

private:
  ~cciMemoryInputStream();

protected:
  dm_uint8* mBuffer;
  dm_uint32 mLength;
  dm_uint32 mOffset;
  dm_uint32 mLeft;
  dm_uint32 mBehaviors;

  dmCString mContentType;
};

#endif /* cciBufferStream_h */
