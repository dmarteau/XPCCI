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

#define dmUserIncludes
#include "dm_utilities.h"
//--------------------------------------------------

#include <stdlib.h>
#include <errno.h>

//--------------------------------------------------
#if defined(DM_MSVCRT_BUILD)

   #include <io.h>
   #include <direct.h>
   #include <process.h>

   #define realpath( _path, _resolved_path )  _fullpath(_resolved_path,_path,MAX_PATH)
   #define stat _stat
   #define access _access
   #define getcwd _getcwd

#else

   #include <unistd.h>
   #include <stdio.h>

   // for access should make sure that:
   // F_OK=0
   // R_OK=4
   // W_OK=2

#endif  // DM_MSVCRT_BUILD

 static dmCString __get_root_dir()
 {
   dmCString::char_type buffer[MAX_PATH];
   getcwd( buffer, MAX_PATH  );
   return dmCString(buffer);
 }


#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#include <map>

//--------------------------------------------------
// ENVIRONMENT VARIABLES
//--------------------------------------------------
typedef std::map<dmCString,dmCString> t_environ;
static t_environ* __dm_Environ = NULL;
#define dmASSERT_VALID_ENVDATA() DM_ASSERTION( __dm_Environ != NULL, "No Environment table !!" )
#define dmENV_TABLE() *__dm_Environ

//--------------------------------------------------
void __dm_ClearEnvironmentTable()
{
  if(__dm_Environ)
  {
    __dm_Environ->clear();
    
    __dm_Environ->~t_environ();
    ::operator delete(__dm_Environ,dm_arena);
    
    __dm_Environ = NULL;
  }
}
//--------------------------------------------------
void __dm_InitEnvironmentTable()
{
  DM_PRECONDITION( __dm_Environ == NULL, "Attempt to init environment twice !!");
  __dm_Environ = new  (dm_arena) t_environ();
}
//--------------------------------------------------
#define MAX_ARGS 50
#define _QUOTE '\"'
#define _WHSP  ' '
#define __EOS  '\0'

#define __null_string( s ) ((s)==0 || (s)[0]==__EOS)

//--------------------------------------------------
void dmPortability::RenameFile(const E* src, const E* dst)
{
  if(__null_string(src) || __null_string(dst))
    return;

  if(rename(src,dst)!=0) {
    dmThrowSystemError(dmCString("Cannot rename ")
      .Append(src).Append(" to ")
      .Append(dst),
      errno);
  }
}
//--------------------------------------------------
void dmPortability::RemoveFile(const E* _filename)
{
  if(__null_string(_filename))
    return;

  if(remove(_filename)!=0) {
    dmThrowSystemError(dmCString("Cannot remove ").Append(_filename),errno);
  }
}
//--------------------------------------------------
bool dmPortability::AccessFile(const E* _path, dm_int _mode)
{
  if(__null_string(_path))
    return false;

  return access(_path,_mode)==0;
}
//----------------------------------------------------------------------------
dmCString dmPortability::SearchEnv(const E* _env,const E* _fileName,
                                   const E* _separators )
{
  if(__null_string(_env) || __null_string(_fileName))
     return dmCString::Null();

  // Check existence of the file
  if(dmPortability::AccessFile(_fileName,dmPortability::eEXISTS))
     return dmCString(_fileName);

  dmCString envvalue = dmPortability::GetEnv(_env);
  if(envvalue.IsEmpty())
     return dmCString::Null();

  dmCString pathname,buffer;

  size_t pos = 0;
  do {
    pos = envvalue.Split(buffer,pos,_separators);
    if(!buffer.IsEmpty())
    {
      buffer.Trim("/\\",false,true);
      pathname = dmCString(buffer).Append(dmPortability::PathSeparator()).Append(_fileName);
      if(dmPortability::AccessFile(pathname.get(),dmPortability::eEXISTS)) {
        #ifdef DM_CONFIG_TARGET_WIN32
        // Normalize path
        // XXX May be Normalize() should be dmCString function ?
         pathname.ReplaceChars("/",'\\');
        #endif
        return pathname;
      }
    }
  } while(!buffer.IsEmpty());

  return dmCString::Null();
}
//----------------------------------------------------------------------------
dmCString dmPortability::SearchDir(const E* _dir,const E* _fileName )
{
  if(__null_string(_dir) || __null_string(_fileName))
    return dmCString::Null();

  dmCString pathname,buffer;

  buffer.Assign(_dir);
  if(!buffer.IsEmpty())
  {
    buffer.Trim("/\\",false,true);
    pathname = dmCString(buffer).Append(dmPortability::PathSeparator()).Append(_fileName);
      if(dmPortability::AccessFile(pathname.get(),dmPortability::eEXISTS))
        return pathname;
  }
  return dmCString::Null();
}
//--------------------------------------------------
dmTime dmPortability::GetFileTime(const dmCString::char_type* _path)
{
  dmASSERT( !__null_string(_path) );

  struct stat s;        // results of stat()

  struct tm* filedate;
  time_t tm_t=0;

  if (stat(_path,&s)==0)
  {
     tm_t = s.st_mtime;
     filedate = localtime(&tm_t);
     return dmTime(filedate->tm_mday,filedate->tm_mon+1,filedate->tm_year+1900,
                   filedate->tm_hour,filedate->tm_min  ,filedate->tm_sec);
  } else
    dmThrowSystemError(dmCString("Cannot get file time for ").Append(_path),errno);
}
//--------------------------------------------------
dm_size dmPortability::GetFileSize(const E* _path)
{
  if(!__null_string(_path) ) {
    struct stat s;  // results of stat()
    if (stat(_path,&s)==0)
      return s.st_size;
  }
  return 0;
}
//--------------------------------------------------
dmCString dmPortability::FullPath( const E* _path )
{
  if(!__null_string(_path)) {
    dmCString::char_type resolvedPath[MAX_PATH];
    return dmCString(realpath(_path,resolvedPath));
  }

  return dmCString::Null();
}
//--------------------------------------------------
dmCString dmPortability::GetCurrentDir()
{
  return __get_root_dir();
}
//--------------------------------------------------
dmCString dmPortability::GetEnv( const E* _VarName )
{
  dmASSERT_VALID_ENVDATA();
  dmASSERT( !__null_string(_VarName) );

  dmCString VarName(_VarName);
  t_environ& _env = dmENV_TABLE();
  t_environ::iterator it = _env.find(VarName);
  if(it == _env.end()) {
     const dmCString::char_type* sz = getenv(VarName.get());
     if(sz!=NULL)
        return (_env[VarName] =  sz);
     else
        return dmCString::Null();
  } else
    return (*it).second;
}
//--------------------------------------------------
void dmPortability::SetEnv( const E* _VarName, const E* _Value )
{
  dmASSERT_VALID_ENVDATA();
  //-----------------------------------------
  // On a besoin de garder des entrees statiques
  // sur les variables passees a _putenv
  //-----------------------------------------
  dmASSERT( !__null_string(_VarName) );
  t_environ& _env = dmENV_TABLE();
  _env[dmCString(_VarName)] = dmCString(_Value);
}
//--------------------------------------------------
const dmCString::char_type* dmPortability::PathSeparator()
{
  #ifdef DM_CONFIG_TARGET_WIN32
   return "\\";
  #else
   return "/";
  #endif
}
//--------------------------------------------------
const dmCString::char_type* dmPortability::PathEnvSeparator()
{
  #ifdef DM_CONFIG_TARGET_WIN32
   return ";";
  #else
   return ":";
  #endif
}
//--------------------------------------------------
dmCString dmPortability::NormalizePath( const E* path )
{
  dmCString pathname(path);

  pathname.ReplaceString("//",2,"/",1);

  #ifdef DM_CONFIG_TARGET_WIN32
  // Normalize path
   pathname.ReplaceChars("/",'\\');
  #endif

  return pathname;
}

//--------------------------------------------------
dmCString dmPortability::RootDir()
{
  dmCString _RootDir = __get_root_dir();
  dmASSERT( !_RootDir.IsEmpty() );
  return _RootDir;
}
//--------------------------------------------------

