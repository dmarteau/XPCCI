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
// File         : cciFileStreams.cpp
// Date         : 26 nov. 2008
// Author       : David Marteau
//--------------------------------------------------------

#include "cciString.h"
#include "cciFileStreams.h"

#include <string.h> // for strerror

/////////////////////////////////////////////////////////////////

// cciFileStreamBase

cciFileStreamBase::cciFileStreamBase()
: mFile(0)
{}

cciFileStreamBase::~cciFileStreamBase()
{
  CloseStream();
}

cci_result cciFileStreamBase::CloseStream()
{
  if(mFile) {
    fclose(mFile);
    mFile = dm_null;
  }

  mSpec.Clear();

  return CCI_OK;
}


//=============================
// cciISeekableStream
//=============================

cci_result cciFileStreamBase::Seek(dm_int32 whence, dm_uint32 offset)
{
  if(!mFile)
     return CCI_ERROR_NOT_INITIALIZED;

  if(fseek(mFile,offset,whence)) {
     dmLOG_ERROR("Cannot seek file %s : %s\n",mSpec.get(),strerror( errno ));
     return CCI_ERROR_FAILURE;
  }

  return CCI_OK;
}

/* void setEOF (); */
cci_result cciFileStreamBase::SetEOF()
{
  return Seek(cciISeekableStream::DM_SEEK_END,0);
}

/* dm_uint32 tell (); */
cci_result cciFileStreamBase::Tell( dm_uint32* _result )
{
  if(!mFile)
     return CCI_ERROR_NOT_INITIALIZED;

  long rv = ftell(mFile);
  if(rv==-1) {
    dmLOG_ERROR("Cannot tell file %s : %s\n",mSpec.get(),strerror( errno ));
    return CCI_ERROR_FAILURE;
  }

  *_result = rv;
  return CCI_OK;
}

/////////////////////////////////////////////////////////////////

// cciFileInputStream

CCI_IMPL_ISUPPORTS3(cciFileInputStream, cciIFileInputStream,
                                        cciIInputStream,
                                        cciISeekableStream)

cciFileInputStream::cciFileInputStream()
{
  /* member initializers and constructor code */
}

cciFileInputStream::~cciFileInputStream()
{
  /* destructor code */
}

/* void init (in string spec, in dm_uint32 ioFlags, in dm_uint32 perm, in dm_uint32 behaviors); */
CCI_IMETHODIMP cciFileInputStream::Init(const char * spec, dm_uint32 ioFlags, dm_uint32 perm, dm_uint32 behaviors)
{
  if(mFile)
    return CCI_ERROR_ALREADY_INITIALIZED;

  mSpec = spec;
  mFile = fopen(mSpec.get(),"rb");

  if(mFile)
     return CCI_OK;

  dmLOG_ERROR("Cannot open file %s : %s\n",mSpec.get(),strerror( errno ));
  return CCI_ERROR_FAILURE;
}

/* [notxpcom] FILE getANSIFileDesc (); */
CCI_IMETHODIMP_(FILE *) cciFileInputStream::GetANSIFileDesc()
{
  return mFile;
}

//=============================
// cciIInputStream
//=============================

/* [notxpcom] dm_uint32 available (); */
CCI_IMETHODIMP_(dm_uint32) cciFileInputStream::Available()
{
  return CCI_ERROR_NOT_IMPLEMENTED;
}

/* void close (); */
CCI_IMETHODIMP cciFileInputStream::Close()
{
  return CloseStream();
}

/* dm_uint32 read ([array, size_is (count)] in octet data, in dm_uint32 count); */
CCI_IMETHODIMP cciFileInputStream::Read(dm_uint8 *buffer, dm_uint32 count, dm_uint32 *_retval )
{
  if(!mFile)
     return CCI_ERROR_NOT_INITIALIZED;

  dm_int byteread = 0;

  if(buffer)
  {
    byteread =  fread(buffer,1,count,mFile);
    if(byteread == -1)
    {
      if(feof(mFile)) {
         clearerr(mFile);
         byteread = 0;
      } else {
        dmLOG_ERROR("Cannot read file %s : %s\n",mSpec.get(),strerror( errno ));
        return CCI_ERROR_FAILURE;
      }
    }
  }
  else
  {
    if(fseek(mFile,count,SEEK_CUR))
    {
      dmLOG_ERROR("Cannot seek/read file %s : %s\n",mSpec.get(),strerror( errno ));
      byteread = 0;
      return CCI_ERROR_FAILURE;
    } else
      byteread = count;
  }

  *_retval = byteread;
  return CCI_OK;

}

/* dm_uint32 readSegments (in cciWriteSegmentFun writer, in voidPtr closure, in dm_uint32 count); */
CCI_IMETHODIMP cciFileInputStream::ReadSegments(cciWriteSegmentFun writer,
                                                void *closure,
                                                dm_uint32 count,
                                                dm_uint32 *_retval )
{
  return CCI_ERROR_NOT_IMPLEMENTED;
}

/* [notxpcom] boolean isNonBlocking (); */
CCI_IMETHODIMP_(bool) cciFileInputStream::IsNonBlocking()
{
  return false;
}

/* [notxpcom] string getContentType (); */
CCI_IMETHODIMP_(const char *) cciFileInputStream::GetContentType()
{
  return mContentType.get();
}

/* [notxpcom] void setContentType (in string type); */
CCI_IMETHODIMP_(void) cciFileInputStream::SetContentType(const char * type)
{
  mContentType.Assign(type);
}

///////////////////////////////////////////////////////////////////////

// cciFileOutputStream

CCI_IMPL_ISUPPORTS3(cciFileOutputStream, cciIFileOutputStream,
                                         cciIOutputStream,
                                         cciISeekableStream)

cciFileOutputStream::cciFileOutputStream()
{
  /* member initializers and constructor code */
}

cciFileOutputStream::~cciFileOutputStream()
{
  /* destructor code */
}

/* void init (in string spec, in dm_uint32 ioFlags, in dm_uint32 perm, in dm_uint32 behaviors); */
CCI_IMETHODIMP cciFileOutputStream::Init(const char * spec, dm_uint32 ioFlags, dm_uint32 perm, dm_uint32 behaviors)
{
  if(mFile)
    return CCI_ERROR_ALREADY_INITIALIZED;

  mSpec = spec;
  mFile = fopen(mSpec.get(),(ioFlags & CCI_FILE_IO_APPEND) ? "ab" : "wb");

  if(mFile)
     return CCI_OK;

  dmLOG_ERROR("Cannot open file %s : %s\n",mSpec.get(),strerror( errno ));
  return CCI_ERROR_FAILURE;
}

/* [notxpcom] FILE getANSIFileDesc (); */
CCI_IMETHODIMP_(FILE *) cciFileOutputStream::GetANSIFileDesc()
{
  return mFile;
}

//=============================
// cciOutputStream
//=============================

/* void close (); */
CCI_IMETHODIMP cciFileOutputStream::Close()
{
  return CloseStream();
}

/* void flush (); */
CCI_IMETHODIMP cciFileOutputStream::Flush()
{
  if(!mFile)
     return CCI_ERROR_NOT_INITIALIZED;

  if(fflush(mFile)!=0) {
    dmLOG_ERROR("Cannot flush file %s : %s\n",mSpec.get(),strerror( errno ));
    return CCI_ERROR_FAILURE;
  }

  return CCI_OK;
}

/* dm_uint32 write ([array, size_is (count)] in octet data, in dm_uint32 count); */
CCI_IMETHODIMP
cciFileOutputStream::Write(dm_uint8 *data, dm_uint32 count, dm_uint32 *_retval )
{
  if(!mFile)
     return CCI_ERROR_NOT_INITIALIZED;

  if(data)
  {
    if((*_retval = fwrite(data,1,count,mFile))!=count) {
      dmLOG_ERROR("Cannot write into file %s : %s\n",mSpec.get(),strerror( errno ));
      return CCI_ERROR_FAILURE;
    }
  }
  else
  {
    for(size_t i=0;i<count;++i) {
      if(fputc(0,mFile)==EOF) {
        dmLOG_ERROR("Cannot write into file %s : %s\n",mSpec.get(),strerror( errno ));
        return CCI_ERROR_FAILURE;
      }
    }
  }

  return CCI_OK;
}


/* dm_uint32 writeFrom (in cciIInputStream fromStream, in dm_uint32 count); */

static CCI_METHOD writeSegmentCallback(cciIInputStream *aInStream,
                                       void *aClosure,
                                       const dm_uint8 *aFromSegment,
                                       dm_uint32 aToOffset,
                                       dm_uint32 aCount,
                                       dm_uint32 *aWriteCount)

{
  cciFileOutputStream* self = reinterpret_cast<cciFileOutputStream*>(aClosure);
  return self->Write(const_cast<dm_uint8*>(aFromSegment),aCount,aWriteCount);
}

CCI_IMETHODIMP cciFileOutputStream::WriteFrom(cciIInputStream *fromStream, dm_uint32 count, dm_uint32 *_retval )
{
  CCI_ENSURE_ARG_POINTER(fromStream);

  return fromStream->ReadSegments(writeSegmentCallback,this,count,_retval);
}

/* unsigned long writeSegments (in cciReadSegmentFun aReader, in voidPtr aClosure, in dm_uint32 aCount); */
CCI_IMETHODIMP cciFileOutputStream::WriteSegments(cciReadSegmentFun aReader,
                                                  void *aClosure,
                                                  dm_uint32 aCount,
                                                  dm_uint32 *_retval )
{
  return CCI_ERROR_NOT_IMPLEMENTED;
}

/* [notxpcom] boolean isNonBlocking (); */
CCI_IMETHODIMP_(bool) cciFileOutputStream::IsNonBlocking()
{
  return false;
}

/* [notxpcom] string getContentType (); */
CCI_IMETHODIMP_(const char *) cciFileOutputStream::GetContentType()
{
  return mContentType.get();
}

/* [notxpcom] void setContentType (in string type); */
CCI_IMETHODIMP_(void) cciFileOutputStream::SetContentType(const char * type)
{
  mContentType.Assign(type);
}

/////////////////////////////////////////////////////////////////////

cci_result CCI_NewFileInputStream( cciIFileInputStream** result,
                                   const char* spec,
                                   dm_uint32 ioFlags,
                                   dm_uint32 perm, dm_uint32 behavior )
{
  *result = dm_null;

  cciFileInputStream* newStream = new cciFileInputStream();
  if(!newStream)
     return CCI_ERROR_OUT_OF_MEMORY;

  cci_result rv = newStream->Init(spec,ioFlags,perm,behavior);
  if(CCI_FAILED(rv)) {
    delete newStream;
  } else {
    CCI_ADDREF(*result = newStream);
  }

  return rv;
}


cci_result CCI_NewFileOutputStream( cciIFileOutputStream** result,
                                    const char* spec,
                                    dm_uint32 ioFlags,
                                    dm_uint32 perm, dm_uint32 behavior )
{
  *result = dm_null;

  cciFileOutputStream* newStream = new cciFileOutputStream();
  if(!newStream)
     return CCI_ERROR_OUT_OF_MEMORY;

  cci_result rv = newStream->Init(spec,ioFlags,perm,behavior);
  if(CCI_FAILED(rv)) {
    delete newStream;
  } else {
    CCI_ADDREF(*result = newStream);
  }

  return rv;
}


