#ifndef dmPortability_h
#define dmPortability_h

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
// File         : dmPortability.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

class __dmUtilities dmPortability
{
  public:
    typedef dmCString::char_type E;

    typedef enum {
      eEXISTS = 0, //  Existence
      eWRITE  = 2, //  Write permission
      eREAD   = 4, //  Read permission
      eALL_ACCESS = eWRITE|eREAD
    } EACCESS_MODE;

    typedef enum {
      eDIRECTORY = 1,
      eFILE      = 2
    } EFILE_TYPE;

  	static void RenameFile(const E* src, const E* dst);
    static void RemoveFile(const E* filename);
    static bool AccessFile(const E* path,dm_int _mode);

    static dmTime   GetFileTime(const E* path);
    static dm_size  GetFileSize(const E* path);

    static dmCString FullPath( const E* path );
    static dmCString GetCurrentDir();

    static dmCString GetEnv( const E* _VarName );
    static void      SetEnv( const E* _VarName, const E* _Value );

    static const E* PathSeparator();
    static const E* PathEnvSeparator();

    static dmCString RootDir();

    static dmCString SearchDir(const E* _dir,const E* _fileName );
    static dmCString SearchEnv(const E* _env,const E* _fileName,
                               const E* _separator = PathEnvSeparator());

    static dmCString  NormalizePath( const E* path );
};
//----------------------------------------------------
#endif // dmPortability_h
