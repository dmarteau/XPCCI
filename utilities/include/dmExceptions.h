#ifndef dmExceptions_h
#define dmExceptions_h

/* ::: BEGIN LICENSE BLOCK:::
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
 *  ::: END LICENSE BLOCK::: */

//--------------------------------------------------------
// File         : dmExceptions.h
// Date         : 6/2004
// Author       : David Marteau
//--------------------------------------------------------

// dmException

class __dmUtilities dmException
{
  public:
    typedef DM_CALLBACK_(void,LogFunction)(const char*,dm_int32 level);

  private:

    static LogFunction _Log;
    static bool        _ActiveLog;

    dmCString _LogMessage;

  protected:
	  const dmCString& LogMessage() const;

  public:
    dmException();
    dmException(const char*      LogMessage);
    dmException(const dmCString& LogMessage);
    dmException(const dmException&);
    virtual ~dmException();

    static void ActivateLog();
    static void DeactivateLog();

    static LogFunction SetLogFunction( LogFunction );

    void Log() const;
    void LogMessage(const char*      aMessage, dm_int32 level = DM_LOG_MESSAGE) const;
    void LogMessage(const dmCString& aMessage, dm_int32 level = DM_LOG_MESSAGE) const;

    static void Trace( dm_int32 level, const char* _fmt ... );
    static void Trace( dm_int32 level, const dmCString& );

    virtual dmCString ErrorCode() const;
};

// dmMsgException

class __dmUtilities dmMsgException: public dmException
{
  private:
  	int       e_code;
  	dmCString e_msg;

  protected:
    void SetCode( int _code );
    void SetMsg ( const dmCString& _msg );

  public:
    dmMsgException();
    dmMsgException( const char* _msg, const char* _text, int _code = -1);
    dmMsgException( const char* _msg, int _code = -1 );
    dmMsgException( const dmCString& _msg, int _code = -1 );
  	dmMsgException( const dmMsgException& );

    const dmMsgException& operator=(const dmMsgException&);

    virtual ~dmMsgException() {};

    int Code() const { return e_code; }

    const dmCString& Msg()  const { return e_msg; }
    const dmCString& Text() const { return LogMessage(); }

    virtual dmCString ErrorCode() const { return dmCString::FormatString(10,"%d",Code()); }
};

// dmSystemError

class __dmUtilities dmSystemError : public dmMsgException
{
  public:
    dmSystemError( const char* _msg, int _code );
    dmSystemError( const dmCString& _msg, int _code );
    virtual ~dmSystemError() {};
};

#define dmThrowSystemError( _msg, _code )  throw dmSystemError(_msg,_code)

#endif // dmExceptions_h
