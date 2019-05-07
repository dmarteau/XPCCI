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

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include "dm_utilities.h"
#include "dmMutex.h"

bool dmException::_ActiveLog = false;

static void DM_STDCALL __dmDfltlogFunction(const char *_buffer,dm_int32 level)
{
  static dmMutex _writemutex;

  dmCRITICAL_SECTION( _writemutex )

  if(_buffer != NULL)
  {
    // Write the message to stderr
    switch(level) {
      case DM_LOG_MESSAGE   : break;
      case DM_LOG_WARNING   : fprintf(stderr, "*** DAIM WARNING : "); break;
      case DM_LOG_ERROR     : fprintf(stderr, "*** DAIM ERROR   : "); break;
      default:
        fprintf(stdout, "*** DAIM LOG(%d)  : ",level); break;
    }
    fprintf(stdout, "%s\n",_buffer);
    fflush(stdout);
  }
}


dmException::LogFunction dmException::_Log = __dmDfltlogFunction;
//---------------------------------------------------------------------------
dmException::dmException() {}
//---------------------------------------------------------------------------
dmException::dmException(const dmException& _toCopy)
  : _LogMessage(_toCopy._LogMessage)
{
}
//---------------------------------------------------------------------------
dmException::dmException(const char* aLogMessage)
: _LogMessage(dmCString("Daim Exception : ").Append(aLogMessage))
{
  Log();
}
//---------------------------------------------------------------------------
dmException::dmException(const dmCString& aLogMessage)
: _LogMessage(dmCString("Daim Exception : ").Append(aLogMessage))
{
  Log();
}
//---------------------------------------------------------------------------
dmException::~dmException()
{}
//---------------------------------------------------------------------------
dmCString dmException::ErrorCode() const
{
	return LogMessage();
}
//---------------------------------------------------------------------------
const dmCString& dmException::LogMessage() const
{
  return _LogMessage;
}
//---------------------------------------------------------------------------
void dmException::Log() const
{
  LogMessage(LogMessage().get(),DM_LOG_WARNING);
}
//---------------------------------------------------------------------------
void dmException::ActivateLog()
{
  _ActiveLog  = true;
  Trace(DM_LOG_MESSAGE,dmCString("Daim Log Enabled at ")
        .Append(dmTime::Now().ToString())
        .Append("\n\n")
        .Append( dmEnvironment::VersionStr())
        .Append("\n"));
}
//---------------------------------------------------------------------------
void dmException::DeactivateLog()
{
  _ActiveLog = false;
}
//---------------------------------------------------------------------------
dmException::LogFunction dmException::SetLogFunction( dmException::LogFunction _f )
{
  LogFunction old = _Log;
  _Log = _f;

  if(_Log==NULL) {
     _Log = __dmDfltlogFunction;
  }

  return old;
}
//----------------------------------------------------------------------------
void dmException::LogMessage(const char* aMessage, dm_int32 level) const
{
  if (_Log && _ActiveLog) {
      _Log( aMessage, level );
  }
}
//----------------------------------------------------------------------------
void dmException::LogMessage(const dmCString& aMessage, dm_int32 level ) const
{
  LogMessage(aMessage.get(),level);
}
//-----------------------------------------------------------------------------
#define DM_LOG_BUFFER_SIZE 4096

void dmException::Trace( dm_int32 level, const char* fmt ... )
{
  static dmCString::char_type __buffer[DM_LOG_BUFFER_SIZE];
  static dmMutex     __LogMutex;

  if (_Log && _ActiveLog)
  {
    dmCRITICAL_SECTION( __LogMutex )

    va_list ap;
    va_start(ap, fmt);
    vsnprintf(__buffer,DM_LOG_BUFFER_SIZE,fmt,ap);
    _Log(__buffer,level);
    va_end(ap);
  }
}
//---------------------------------------------------------------------------
void dmException::Trace( dm_int32 level, const dmCString& aString )
{
  if (_Log && _ActiveLog) {
      _Log( aString.get(), level );
      _Log( "\n", DM_LOG_MESSAGE );
  }
}
//---------------------------------------------------------------------------
dmMsgException::dmMsgException() : e_code(-1) {}
//---------------------------------------------------------------------------
dmMsgException::dmMsgException( const char* _msg, const char* _text, int _code )
: dmException(dmCString( _msg ).Append(" - ").Append(_text))
, e_code(_code)
, e_msg(_text)
{
}
//---------------------------------------------------------------------------
dmMsgException::dmMsgException( const char* _msg, int _code  )
: dmException(_msg), e_code(_code), e_msg(_msg)
{
}
//---------------------------------------------------------------------------
dmMsgException::dmMsgException( const dmCString& _msg, int _code  )
: dmException(_msg), e_code(_code), e_msg(_msg)
{
}
//---------------------------------------------------------------------------
dmMsgException::dmMsgException( const dmMsgException& _E ) : dmException(_E)
{
  e_code = _E.e_code;
	e_msg  = _E.e_msg;
}
//---------------------------------------------------------------------------
const dmMsgException& dmMsgException::operator=(const dmMsgException& _E)
{
	e_code      = _E.e_code;
	e_msg       = _E.e_msg;
	return (*this);
}
//---------------------------------------------------------------------------
void dmMsgException::SetCode( int _code )
{
	e_code = _code;
}
//---------------------------------------------------------------------------
void dmMsgException::SetMsg ( const dmCString& _msg )
{
	e_msg = _msg;
}
//---------------------------------------------------------------------------
#include <string.h>
//---------------------------------------------------------------------------
dmSystemError::dmSystemError( const char* _msg, int _code  )
: dmMsgException(_msg,strerror( _code ),_code)
{
}
//---------------------------------------------------------------------------
dmSystemError::dmSystemError( const dmCString& _msg, int _code  )
: dmMsgException(_msg.get(),strerror( _code ),_code)
{
}
//---------------------------------------------------------------------------
