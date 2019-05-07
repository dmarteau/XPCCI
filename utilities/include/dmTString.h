
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
// File         : dmString.h
// Date         : 4/2008
// Author       : David Marteau
//--------------------------------------------------------

class dmTString_Incompatible_Type;

class __dmUtilities dmTAString_CharT
{
  public:
    typedef CharT               char_type;
    typedef other_CharT         other_char_type;
    typedef dmTAString_CharT    self_type;
    typedef dm_uint32           size_type;
    typedef dm_uint32           index_type;

    enum { npos = DM_UINT32_MAX };

  private:
    dmTString_implementation* _impl;

    void create_local_copy();

  public:
    static dmTAString_CharT Null();
    static dmTAString_CharT Concat( const char_type* , const char_type*  );
    static dmTAString_CharT Concat( const char_type* , char_type         );
    static dmTAString_CharT Concat( char_type        , const char_type*  );
    static dmTAString_CharT FormatString( size_type n, const char_type* fmt ... );

  public:
    dmTAString_CharT();
    dmTAString_CharT( size_type n, char_type c );
    dmTAString_CharT( const self_type& aString );

    explicit dmTAString_CharT( const char_type*  aString );
    explicit dmTAString_CharT( const other_char_type* s );

    ~dmTAString_CharT();

    void  Clear();
    void  Release();

    const char_type* get() const;

    size_type Size()    const;
    bool      IsEmpty() const;

    self_type& operator=(const char_type* aString)  { return Assign(aString); }
    self_type& operator=(const self_type& aString)  { return Assign(aString); }
    self_type& operator=(char_type aChar)           { return Assign(aChar);   }

    bool IsEqual( const char_type* s )   const { return Compare(s)==0;   }
    bool IsEqual( const self_type& str ) const { return Compare(str)==0; }

    bool operator==( const char_type* s ) const { return Compare(s)==0; }
    bool operator!=( const char_type* s ) const { return Compare(s)!=0; }
    bool operator<=( const char_type* s ) const { return Compare(s)<=0; }
    bool operator>=( const char_type* s ) const { return Compare(s)>=0; }
    bool operator< ( const char_type* s ) const { return Compare(s)< 0; }
    bool operator> ( const char_type* s ) const { return Compare(s)> 0; }

    bool operator==( const self_type& str ) const { return Compare(str)==0; }
    bool operator!=( const self_type& str ) const { return Compare(str)!=0; }
    bool operator<=( const self_type& str ) const { return Compare(str)<=0; }
    bool operator>=( const self_type& str ) const { return Compare(str)>=0; }
    bool operator< ( const self_type& str ) const { return Compare(str)< 0; }
    bool operator> ( const self_type& str ) const { return Compare(str)> 0; }

    // conversion operator
    int           ToInt     (int _base=10) const; // same as strtol
    unsigned int  ToUnsigned(int _base=10) const; // same as strtoul;

    const char_type* Begin() const;
    const char_type* End()   const;

    self_type& Trim( const char_type* _tokens  = NULL, bool _leading  = true,
                                               bool _trailing = true );

    // Parse will split a string into components separated by
    // characters from <_separators>, it returns a string for each characters
    // found in string. That is, two consecutives characters will return a empty
    // string
    size_type Parse( self_type& _dest, index_type pos, const char_type* _separators ) const;

    // Split will split a string into components separated by
    // any consecutives numbers of characters from <_separators>
    size_type Split( self_type& _dest, index_type pos, const char_type* _separators ) const;

    self_type& Format(  size_type n, const char_type* fmt ... );

    index_type Find(char_type c, index_type pos = 0)                  const;
    index_type Find(const char_type *s, index_type pos = 0)           const;
    index_type Find(const char_type *s, index_type pos, size_type n)  const;
    index_type Find(const self_type& str, index_type pos = 0)         const {
      return Find(str.get(),pos,str.Size());
    }

    index_type FindFirstOf(char_type c, index_type pos = 0)          const;
    index_type FindFirstOf(const char_type* s, index_type pos = 0)   const;
    index_type FindFirstOf(const self_type& str, index_type pos = 0) const {
      return FindFirstOf(str.get(),pos);
    }

    index_type FindFirstNotOf(char_type c, size_type pos = 0)          const;
    index_type FindFirstNotOf(const char_type* s, size_type pos = 0)   const;
    index_type FindFirstNotOf(const self_type& str, size_type pos = 0) const {
      return FindFirstNotOf(str.get(),pos);
    }

    index_type FindLastOf(char_type c, index_type pos = npos)          const;
    index_type FindLastOf(const char_type* s, index_type pos = npos)   const;
    index_type FindLastOf(const self_type& str, index_type pos = npos) const {
      return FindLastOf(str.get(),pos);
    }

    self_type& Replace(size_type p0, size_type n0, const char_type* s);
    self_type& Replace(size_type p0, size_type n0, const char_type* s, size_type n);
    self_type& Replace(size_type p0, size_type n0, const self_type& str, size_type pos, size_type n);
    self_type& Replace(size_type p0, size_type n0, const self_type& str ) {
      return Replace(p0,n0,str.get(),str.Size());
    }

    self_type& Assign(const char_type *s, size_type n = npos);
    self_type& Assign(const self_type& str, index_type pos, size_type n);
    self_type& Assign(const self_type& str);
    self_type& Assign(char_type c);

    self_type& Assign( const other_char_type* s, size_type n = npos  );
    self_type& Assign( const dmTString_Incompatible_Type& str );

    self_type& Insert(index_type p0, const char_type* s);
    self_type& Insert(index_type p0, const char_type* s, size_type n);
    self_type& Insert(index_type p0, const self_type& str);
    self_type& Insert(index_type p0, const self_type& str, index_type pos, size_type n);
    self_type& Insert(index_type p0, char_type c) {
      return Insert(p0,&c,1);
    }

    self_type& Fill(index_type p0, size_type n, char_type c);

    self_type& Append(const char_type *s)                               { return Insert(npos,s);         }
    self_type& Append(const char_type *s, size_type n)                  { return Insert(npos,s,n);       }
    self_type& Append(const self_type& str,index_type pos, size_type n) { return Insert(npos,str,pos,n); }
    self_type& Append(const self_type& str)                             { return Insert(npos,str);       }
    self_type& Append(char_type c)                                      { return Insert(npos,c);         }

    size_type ReplaceString( const char_type* _seq,size_type _len1, const char_type* _replace, size_type _len2 );
    size_type ReplaceString( const self_type& _seq, const self_type& _replace );

    size_type ReplaceChars ( const char_type* _chars, char_type  c );

    char_type  operator[]( index_type ) const;
    char_type& operator[]( index_type );

    int Compare(const char_type* s, size_type n=npos, bool _Case=true) const;
    int Compare(const self_type& str,bool _Case=true) const {
        return Compare(str.get(),str.Size(),_Case);
    }

    self_type SubStr( index_type pos = 0, size_type n = npos ) const;

    self_type& ToLower();
    self_type& ToUpper();

};

typedef dmTAString_CharT  dmTString_Type;
