#ifndef dmErrors_h
#define dmErrors_h

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
// File         : dmErrors.h
// Date         : 6/2004
// Author       : David Marteau
//--------------------------------------------------------

typedef dmMsgException dmCustomError;

#define dmBASE_ERROR(x) x

#define dmTHROW( E )    throw E
#define dmTHROW_MSG_EXCEPTION( E, msg ) dmTHROW( E(msg) )

#define dmTRY try {
#define dmCATCH(_E) } catch( dmCustomError& _E ) {
#define dmDONE }

#define dmLOG_INFO() \
  (dmCString::FormatString(dmCString::npos,"FILE %s LINE %d",__FILE__,__LINE__))

#define dmTRACE_EXCEPTION(a) ( a.LogMessage(dmCString("Caught in ").Append(dmLOG_INFO())) )
#define dmTRACE( msg )       dmException::Trace(DM_LOG_MESSAGE,dmLOG_INFO().Append(": ").Append(msg))
#define dmABNORMAL_EXCEPTION dmMsgException("###!!! ABNORMAL_EXCEPTION"), dmLOG_INFO() )

//XXX Preprocessor must support VARIADIC macros
//The following will work for GCC and Visual C++ >= 2005
#define dmLOG( ... )       dmException::Trace(DM_LOG_MESSAGE, __VA_ARGS__)
#define dmLOG_ERROR( ... ) dmException::Trace(DM_LOG_ERROR  , __VA_ARGS__)
#define dmLOG_WARN( ... )  dmException::Trace(DM_LOG_WARNING, __VA_ARGS__)

#define dmRETHROW(_E)   \
  dmTRACE_EXCEPTION(_E);\
  dmTHROW( _E );

#define dmRETHROW_ERROR\
  dmCATCH(_E)  \
  dmRETHROW(_E)\
  dmDONE

#define dmTHROW_ERROR( _text )  dmTHROW( dmCustomError(_text) )

#define dmASSERT( expr ) if(!(expr)) \
  dmTHROW( dmMsgException(dmCString("###!!! ASSERTION FAILED: ").Append(dmLOG_INFO()).Append(" ").Append(#expr)) )

#if defined(_DEBUG) || defined(DEBUG)
#define dmDEBUG_TRACE( _text )  dmTRACE(__text)
#define dmDEBUG_ASSERT( _expr ) if(!(_expr)) \
  dmDebugBreak(DM_DEBUG_ASSERTION,dm_null,#_expr,__FILE__,__LINE__)
#else
#define dmDEBUG_ASSERT( _pred )
#define dmDEBUG_TRACE( _text )
#endif



//------------------------------------------------------------------------
#endif
