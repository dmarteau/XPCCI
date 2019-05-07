#ifndef cciFileStreams_h
#define cciFileStreams_h
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
// File         : cciFileStreams.h
// Date         : 26 nov. 2008
// Author       : David Marteau
//--------------------------------------------------------

#include "cciIFileStreams.h"

#include <stdio.h>
#include <errno.h>

class cciFileStreamBase
{
protected:
  cciFileStreamBase();
  virtual ~cciFileStreamBase();

  cci_result Seek(dm_int32 whence, dm_uint32 offset);
  cci_result SetEOF();
  cci_result Tell( dm_uint32* _result );


  cci_result CloseStream();

protected:
  FILE*     mFile;
  dmCString mSpec;
  dmCString mContentType;

};


#define CCI_FILEINPUTSTREAM_CID \
{ /* EBB539BD-FCE9-4E17-B08A-805A8FA7C1B2 */       \
  0xEBB539BD,                                      \
  0xFCE9,                                          \
  0x4E17,                                          \
  {0xB0, 0x8A, 0x80, 0x5A, 0x8F, 0xA7, 0xC1, 0xB2} \
}

#define CCI_FILEINPUTSTREAM_CONTRACTID "@daim.org/contrib/file-input-stream;1"
#define CCI_FILEINPUTSTREAM_CLASSNAME  "File input stream"

class cciFileInputStream : public cciFileStreamBase,
                           public cciIFileInputStream,
                           public cciISeekableStream
{
public:
  CCI_DECL_ISUPPORTS
  CCI_DECL_IFILEINPUTSTREAM
  CCI_DECL_IINPUTSTREAM

  CCI_FORWARD_ISEEKABLESTREAM(cciFileStreamBase::)

   cciFileInputStream();
  ~cciFileInputStream();

protected:

};


#define CCI_FILEOUTPUTSTREAM_CID \
{ /* 6B71E830-2A2F-4DE6-84E4-C9ACA24FED36 */       \
  0x6B71E830,                                      \
  0x2A2F,                                          \
  0x4DE6,                                          \
  {0x84, 0xE4, 0xC9, 0xAC, 0xA2, 0x4F, 0xED, 0x36} \
}

#define CCI_FILEOUTPUTSTREAM_CONTRACTID "@daim.org/contrib/file-output-stream;1"
#define CCI_FILEOUTPUTSTREAM_CLASSNAME  "File output stream"

class cciFileOutputStream : public cciFileStreamBase,
                            public cciIFileOutputStream,
                            public cciISeekableStream
{
public:
  CCI_DECL_ISUPPORTS
  CCI_DECL_IFILEOUTPUTSTREAM
  CCI_DECL_IOUTPUTSTREAM

  CCI_FORWARD_ISEEKABLESTREAM(cciFileStreamBase::)

   cciFileOutputStream();
  ~cciFileOutputStream();

protected:
};

#endif /* cciFileStreams_h */
