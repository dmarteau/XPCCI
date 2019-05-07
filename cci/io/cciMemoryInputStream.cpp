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
// File         : cciBufferStream.cpp
// Date         : 26 nov. 2008
// Author       : David Marteau
//--------------------------------------------------------

#include "cciString.h"
#include "cciMemoryInputStream.h"

/* Implementation file */
CCI_IMPL_ISUPPORTS3(cciMemoryInputStream, cciIMemoryInputStream,
                                          cciIInputStream,
                                          cciISeekableStream)

cciMemoryInputStream::cciMemoryInputStream()
: mBuffer(dm_null)
, mLength(0)
, mOffset(0)
, mLeft(0)
, mBehaviors(0)
{

}

cciMemoryInputStream::~cciMemoryInputStream()
{
  Close();
}

//=============================
// cciIMemoryInputStream
//=============================

/* void init ([array, size_is (length)] in octet data, in dm_uint32 length, in dm_uint32 behaviors); */
CCI_IMETHODIMP cciMemoryInputStream::Init(dm_uint8 *data, dm_uint32 length, dm_uint32 behaviors)
{
  if(mBuffer)
     return CCI_ERROR_ALREADY_INITIALIZED;

  if(length > 0 && data == NULL)
     return CCI_ERROR_INVALID_ARG;

  mBehaviors = behaviors;
  mBuffer    = data;
  
  mLength    = length;
  mLeft      = mLength;

  return CCI_OK;
}

//=============================
// cciISeekableStream
//=============================

/* void seek (in dm_int32 whence, in dm_uint32 offset); */
CCI_IMETHODIMP cciMemoryInputStream::Seek(dm_int32 whence, dm_uint32 offset)
{
  if(!mBuffer)
    return CCI_ERROR_NOT_INITIALIZED;

  switch(whence)
  {
    case DM_SEEK_SET :
      break;

    case DM_SEEK_CUR :
      offset += mOffset;
      break;

    case DM_SEEK_END :
      offset = mLength - offset;
      break;

    default:
      return CCI_ERROR_INVALID_ARG;
  }

  if(offset > mLength)
     return CCI_ERROR_INVALID_ARG;

  mOffset = offset;
  mLeft   = mLength - mOffset;
  return CCI_OK;
}


/* void setEOF (); */
CCI_IMETHODIMP cciMemoryInputStream::SetEOF()
{
  if(!mBuffer)
    return CCI_ERROR_NOT_INITIALIZED;

  mLeft   = 0;
  mOffset = mLength;
  return CCI_OK;
}

/* [notxpcom] dm_uint32 tell (); */
CCI_IMETHODIMP cciMemoryInputStream::Tell(dm_uint32 *_retval )
{
  *_retval = mOffset;
  return CCI_OK;
}


//=============================
// cciIInputStream
//=============================

/* [notxpcom] dm_uint32 available (); */
CCI_IMETHODIMP_(dm_uint32) cciMemoryInputStream::Available()
{
  if(!mBuffer)
    return CCI_ERROR_NOT_INITIALIZED;

  return mLeft;
}

/* void close (); */
CCI_IMETHODIMP cciMemoryInputStream::Close()
{
  if(!mBuffer)
    return CCI_ERROR_NOT_INITIALIZED;

  if(mBehaviors & ADOPT_DATA)
     dmMemory::Free(mBuffer);

  mLength    = 0;
  mOffset    = 0;
  mLeft      = 0;
  mBehaviors = 0;

  mBuffer = dm_null;

  return CCI_OK;
}

/* dm_uint32 read ([array, size_is (count)] in octet data, in dm_uint32 count); */
CCI_IMETHODIMP cciMemoryInputStream::Read(dm_uint8 *buffer, dm_uint32 count, dm_uint32 *_retval )
{
  *_retval = 0;

  if(mBuffer)
  {
    count = dm_min(mLeft,count);

    if(buffer)
      dmMemory::Memcpy(buffer,mBuffer + mOffset,count);

    mOffset += count;
    mLeft   -= count;

    *_retval = count;
  }

  return CCI_OK;
}


/* [noscript] dm_uint32 readSegments (in cciWriteSegmentFun writer, in voidPtr closure, in dm_uint32 count); */
CCI_IMETHODIMP cciMemoryInputStream::ReadSegments(cciWriteSegmentFun writer, void *closure,
                                                  dm_uint32 count,
                                                  dm_uint32 *_retval )
{
  *_retval = 0;

  cci_result rv;
  if(mBuffer)
  {
    while (count > 0)
    {
      dm_uint32 amount = dm_min(mLeft,count);
      if(amount > 0)
      {
        dm_uint32 read = 0;
        rv = writer(this,closure,mBuffer + mOffset,*_retval,amount,&read);
        if(CCI_FAILED(rv)) {
           rv = CCI_OK;
           break;
        }
        *_retval += read;
        count    -= read;

        mOffset  += read;
        mLeft    -= read;
      } else
        break;
    }
  }

  return CCI_OK;
}

/* [notxpcom] boolean isNonBlocking (); */
CCI_IMETHODIMP_(bool) cciMemoryInputStream::IsNonBlocking()
{
  return false;
}

/* [notxpcom] string getContentType (); */
CCI_IMETHODIMP_(const char *) cciMemoryInputStream::GetContentType()
{
  return mContentType.get();
}

/* [notxpcom] void setContentType (in string type); */
CCI_IMETHODIMP_(void) cciMemoryInputStream::SetContentType(const char * type)
{
  mContentType.Assign(type);
}

