
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

template<> const CharT  __dm_string_allocator<CharT>::end_of_string = to_CharT('\0');
template<> const CharT* __dm_string_allocator<CharT>::white_spaces  = to_CharT(" \t\n\r");
template<> const CharT* __dm_string_allocator<CharT>::empty_string  = to_CharT("");

#define dm_end_of_string __dm_string_allocator<CharT>::end_of_string
#define dm_white_spaces  __dm_string_allocator<CharT>::white_spaces
#define dm_empty_string  __dm_string_allocator<CharT>::empty_string

#define is_null_string( s ) ((s)==0 || s[0]==dm_end_of_string)

//------------------------------------------
dmTAString_CharT dmTAString_CharT::FormatString( size_type n, const char_type* fmt ... )
{
  dmTAString_CharT result;
  if(!is_null_string(fmt))
  {
    result.create_local_copy();
    va_list ap;
    va_start(ap, fmt);
    dmTString_implementation::_Format( result._impl, n, fmt, ap );
  }
  return result;
}
//------------------------------------------
dmTAString_CharT dmTAString_CharT::Null()
{
  static const char_type* __nullstr = dm_empty_string;
  return dmTAString_CharT(__nullstr);
}
//------------------------------------------
// static concatenation fonctions
//------------------------------------------
dmTAString_CharT dmTAString_CharT::Concat( const char_type* s1, const char_type* s2 )
{
  dmTAString_CharT _result;

  if(s2==dm_null) s1 = dm_empty_string;
  if(s2==dm_null) s2 = dm_empty_string;

  size_type n1 = _dm_strlen(s1);
  size_type n2 = _dm_strlen(s2);

  _result.create_local_copy();
  _result._impl->Reserve(n1+n2);
  _result._impl->Assign(s1,n1);
  _result._impl->Append(s2,s2+n2);
  return _result;
}
//------------------------------------------
dmTAString_CharT dmTAString_CharT::Concat( char_type c, const char_type* s )
{
  dmTAString_CharT _result;

  if(!is_null_string(s))
  {
    size_type n = _dm_strlen(s);

    _result.create_local_copy();
    _result._impl->Reserve(n+1);
    _result._impl->Assign(c,1);
    _result._impl->Append(s,s+n);
  } else
    _result.Assign(c);

  return _result;
}
//------------------------------------------
dmTAString_CharT dmTAString_CharT::Concat( const char_type* s, char_type c  )
{
  dmTAString_CharT _result;

  if(!is_null_string(s))
  {
    size_type n = _dm_strlen(s);

    _result.create_local_copy();
    _result._impl->Reserve(n+1);
    _result._impl->Assign(s,n);
    _result._impl->Append(c,1);
  } else
    _result.Assign(c);

   return _result;
}
//------------------------------------------
// If we are to modify the buffer
// then create a local copy of the string
// content
//------------------------------------------
void dmTAString_CharT::create_local_copy()
{
  if(_impl)
  {
    if(_impl->References()>1) {
       _impl->Release();
       _impl = new dmTString_implementation(*_impl);
    }
  } else
    _impl = new dmTString_implementation();
}
//------------------------------------------
dmTAString_CharT::~dmTAString_CharT()
{
  if(_impl)
     _impl->Release();
}
//------------------------------------------
dmTAString_CharT::dmTAString_CharT()
:_impl(NULL)
{
}
//------------------------------------------
dmTAString_CharT::dmTAString_CharT( const char_type*  aCString )
:_impl(NULL)
{
  if(aCString!=NULL)
    _impl =  new dmTString_implementation(aCString);
}
//------------------------------------------
dmTAString_CharT::dmTAString_CharT( size_type n, char_type c  )
:_impl(NULL)
{
  if(n>0)
    _impl = new dmTString_implementation(n,c);
}
//------------------------------------------
dmTAString_CharT::dmTAString_CharT( const dmTAString_CharT& aString )
{
  _impl = aString._impl;
  if(_impl)
     _impl->Obtain();
}
//------------------------------------------
dmTAString_CharT::dmTAString_CharT( const other_char_type* s )
:_impl(NULL)
{
  Assign(s);
}
//------------------------------------------
dmTAString_CharT::size_type
dmTAString_CharT::Size() const
{
  return _impl ? _impl->Size() : 0;
}
//------------------------------------------
bool dmTAString_CharT::IsEmpty() const
{
  return _impl ? _impl->Empty() : true;
}
//------------------------------------------
void dmTAString_CharT::Clear()
{
  if(_impl)
  {
    if(_impl->References()==1) {
       _impl->Clear();
    } else {
      _impl->Release();
      _impl = NULL;
    }
  }
}
//------------------------------------------
void dmTAString_CharT::Release()
{
  if(_impl)
  {
    _impl->Release();
    _impl = NULL;
  }
}
//------------------------------------------
const dmTAString_CharT::char_type*
dmTAString_CharT::get() const
{
  if(_impl)
    return _impl->Begin();
  else
    return dm_empty_string;
}
//------------------------------------------
int dmTAString_CharT::ToInt(int _base) const
{
  if(_impl && !_impl->Empty()) {
    char_type* _stopc;
    return static_cast<int>(_dm_strtol(_impl->Begin(),&_stopc,_base));
  } else
    return 0;
}
//------------------------------------------
unsigned int dmTAString_CharT::ToUnsigned(int _base)  const
{
  if(_impl && !_impl->Empty()) {
    char_type* _stopc;
    return static_cast<int>(_dm_strtoul(_impl->Begin(),&_stopc,_base));
  } else
    return 0;
}
//------------------------------------------
const dmTAString_CharT::char_type* dmTAString_CharT::Begin() const
{
  if(_impl)
    return _impl->Begin();

  return NULL;
}
//------------------------------------------
const dmTAString_CharT::char_type* dmTAString_CharT::End() const
{
  if(_impl)
    return _impl->End();

  return NULL;
}
//------------------------------------------
dmTAString_CharT& dmTAString_CharT::Insert(index_type p0, const char_type *s, size_type n)
{
  if(!is_null_string(s))
  {
    create_local_copy();
    if(p0>_impl->Size()) p0 = _impl->Size();
    _impl->Insert(_impl->Begin()+p0,s,s+n);
  }
  return *this;
}
//------------------------------------------
dmTAString_CharT& dmTAString_CharT::Insert(index_type p0, const char_type *s)
{
  if(s!=NULL)
    Insert(p0,s,_dm_strlen(s));

  return *this;
}
//------------------------------------------
dmTAString_CharT& dmTAString_CharT::Insert(index_type p0, const dmTAString_CharT& str)
{
  return Insert(p0,str.get(),str.Size());
}
//------------------------------------------
dmTAString_CharT& dmTAString_CharT::Insert( index_type p0, const dmTAString_CharT& str, index_type pos, size_type n)
{
  if(pos < str.Size())
  {
    if(n+pos > str.Size()) n = str.Size() - pos;
    return Insert(p0,str.Begin()+pos,n );
  } else
    return *this;
}
//------------------------------------------
dmTAString_CharT& dmTAString_CharT::Fill( index_type p0, size_type n, char_type c)
{
  if(n>0)
  {
    create_local_copy();
    if(p0>_impl->Size()) p0 = _impl->Size();
    _impl->Insert(_impl->Begin()+p0,c,n);
  }
  return *this;
}
//------------------------------------------
dmTAString_CharT dmTAString_CharT::SubStr( index_type pos, size_type n ) const
{
  dmTAString_CharT _str;
  if(_impl && pos < _impl->Size() )
    _str.Assign(*this,pos,n);

  return _str;
}
//------------------------------------------
dmTAString_CharT::index_type
dmTAString_CharT::Find( char_type c, index_type pos ) const
{
  return FindFirstOf(c,pos);
}
//------------------------------------------
dmTAString_CharT::index_type
dmTAString_CharT::Find(const char_type *s, index_type pos ) const
{
  return Find(s,pos,_dm_strlen(s));
}
//------------------------------------------
dmTAString_CharT::index_type
dmTAString_CharT::Find(const char_type *s, index_type pos, size_type n) const
{
  if(_impl && pos < _impl->Size() && n>0)
  {
    n = std::min(static_cast<int>(n),
                 static_cast<int>(_impl->Size()) - static_cast<int>(pos) );

    const char_type* _First = _impl->Begin()+pos;
    const char_type* _Last = _impl->End()  -n+1;
    for(;_First!=_Last;++_First)
      if(_dm_strncmp(_First,s,n)==0)
        return _First - _impl->Begin();
  }

  return npos;
}
//------------------------------------------
dmTAString_CharT::index_type
dmTAString_CharT::FindFirstOf(char_type c, index_type pos )  const
{
  if(_impl && pos < _impl->Size() )
  {
    const char_type* _position = std::find(_impl->Begin()+pos,_impl->End(),c);
    if(_position != _impl->End() )
       return _position - _impl->Begin();
  }
  return npos;
}
//------------------------------------------
dmTAString_CharT::index_type
dmTAString_CharT::FindFirstOf(const char_type* s, index_type pos ) const
{
  if(!is_null_string(s) && _impl && pos < _impl->Size() )
  {
    const char_type* _position =
     std::find_if(_impl->Begin()+pos,_impl->End(),___IsCharWithin<char_type>(s));

    if(_position != _impl->End())
     return _position - _impl->Begin();
  }
  return npos;
}
//------------------------------------------
dmTAString_CharT::index_type
dmTAString_CharT::FindLastOf(char_type c, index_type pos )  const
{
  if( _impl && (pos < _impl->Size()||pos==npos) )
  {
    if(pos==npos) pos  = _impl->Size();
    else          pos += 1;

    const char_type* _start = _impl->Begin()+pos;
    const char_type* _end   = _impl->Begin();
    while(_start!=_end) {
      if(*--_start == c)
        return _start - _end;
    }
  }
  return npos;
}
//------------------------------------------
dmTAString_CharT::index_type
dmTAString_CharT::FindLastOf(const char_type* s, index_type pos )  const
{
  if( _impl && (pos < _impl->Size()||pos==npos) )
  {
    if(pos==npos) pos  = _impl->Size();
    else          pos += 1;

    ___IsCharWithin<char_type> _pred(s);

    const char_type* _start = _impl->Begin()+pos;
    const char_type* _end   = _impl->Begin();
    while(_start!=_end) {
      if(_pred(*--_start))
        return _start - _end;
    }
  }
  return npos;
}
//------------------------------------------
dmTAString_CharT::index_type
dmTAString_CharT::FindFirstNotOf(char_type c, index_type pos ) const
{
  if(_impl && pos < _impl->Size())
  {
    const char_type* _First = _impl->Begin()+pos;
    const char_type* _Last = _impl->End();
    for(;_First!=_Last;++_First) {
      if(*_First != c)
        return _First - _impl->Begin();
    }
  }
  return npos;
}
//-------------------------------------------
dmTAString_CharT::index_type
dmTAString_CharT::FindFirstNotOf(const char_type* s, index_type pos ) const
{
  if(!is_null_string(s) && _impl && pos < _impl->Size() )
  {
    const char_type* _position =
     std::find_if(_impl->Begin()+pos,_impl->End(),___IsCharNotWithin<char_type>(s));

    if(_position != _impl->End())
     return _position - _impl->Begin();
  }
  return npos;
}
//-------------------------------------------
dmTAString_CharT& dmTAString_CharT::Assign(const char_type *s, size_type n)
{
  if(is_null_string(s)||n==0)
    Clear();
  else
  {
    if(n==npos)
       n = _dm_strlen(s);

    create_local_copy();
    _impl->Assign(s,n);
  }
  return *this;
}
//-------------------------------------------
dmTAString_CharT& dmTAString_CharT::Assign(const dmTAString_CharT& str, index_type pos, size_type n)
{
  if(pos >= str.Size()||n==0)
    Clear();
  else
  {
    create_local_copy();

    if(n==npos || (n+pos > str.Size()) )
       n = str.Size() - pos;

    _impl->Assign(str.get()+pos,n);
  }
  return *this;
}
//-------------------------------------------
dmTAString_CharT& dmTAString_CharT::Assign(const dmTAString_CharT& str)
{
  if(&str!=this)
  {
    if(_impl)
       _impl->Release();

    _impl = str._impl;
    if(_impl)
       _impl->Obtain();
  }
  return *this;
}
//-------------------------------------------
dmTAString_CharT& dmTAString_CharT::Assign(char_type c)
{
  create_local_copy();
  _impl->Assign(1,c);

  return *this;
}
//-------------------------------------------
dmTAString_CharT& dmTAString_CharT::Assign( const other_char_type* s, size_type n )
{
  if(is_null_string(s))
     Clear();
  else {
    create_local_copy();
    if(n==npos)
       n = _dm_strlen(s);

    _impl->Assign(dm_end_of_string,n);
    #ifdef CharT_is_char
     wcstombs(_impl->Begin(),s,n);
    #else
     mbstowcs(_impl->Begin(),s,n);
    #endif
  }

  return *this;
}

//------------------------------------------
dmTAString_CharT& dmTAString_CharT::Replace(index_type p0, size_type n0, const char_type *s, size_type n)
{
  if(s!=NULL && _impl && p0 < _impl->Size())
  {
    if(n0==npos)
       n0 = _impl->Size() - p0;

    create_local_copy();
    _impl->Erase (_impl->Begin()+p0,_impl->Begin()+std::min(n0+p0,Size()));
    _impl->Insert(_impl->Begin()+p0,s,s+n);
  }
  return *this;
}
//------------------------------------------
dmTAString_CharT& dmTAString_CharT::Replace(index_type p0, size_type n0, const char_type *s)
{
  if(s!=NULL)
    Replace(p0,n0,s,_dm_strlen(s));

  return *this;
}
//------------------------------------------
dmTAString_CharT& dmTAString_CharT::Replace(index_type p0, size_type n0, const dmTAString_CharT& str, index_type pos, size_type n)
{
  size_type strsz = str.Size();
  if(pos < strsz)
  {
    if(n==npos || (n+pos > strsz))
       n = strsz - pos;

    return Replace(p0,n0,str.Begin()+pos,n);
  }
  return *this;
}
//------------------------------------------
dmTAString_CharT& dmTAString_CharT::Trim( const char_type* _tokens, bool _leading, bool _trailing )
{
  if(_impl)
  {
    if(_tokens == NULL)
       _tokens = dm_white_spaces;

    if(_leading)
    {
      char_type* _First = _impl->Begin();
      char_type* _Last  = _impl->End();
      while(_First!=_Last) {
        if(___IsCharWithin<char_type>(_tokens)(*_First))
          ++_First;
        else
          break;
      }
      if(_First!=_impl->Begin()) {
         create_local_copy();
         _impl->Erase(_impl->Begin(),_First);
      }
    }

    if(_trailing)
    {
      char_type* _First = _impl->End()  -1;
      char_type* _Last  = _impl->Begin()-1;
      while(_First!=_Last) {
        if(___IsCharWithin<char_type>(_tokens)(*(_First)))
          --_First;
        else
          break;
      }
      if(_First!= _impl->End()-1 ) {
         create_local_copy();
         _impl->Erase(_First+1,_impl->End());
      }
    }
  }
  return *this;
}
//------------------------------------------
dmTAString_CharT::index_type
dmTAString_CharT::ReplaceChars( const char_type* _chars, char_type _replace )
{
  index_type pos = 0;
  size_type cnt = 0;
  do {
    pos = FindFirstOf(_chars,pos);
    if(pos!=npos) {
      ++cnt;
      (*_impl)[pos] = _replace;
    }
  } while( pos != npos );
  return cnt;
}
//------------------------------------------
dmTAString_CharT::index_type
dmTAString_CharT::ReplaceString( const char_type* _seq, size_type _len1, const char_type* _replace, size_type _len2 )
{
  if(_impl && !is_null_string(_seq))
  {
    if(_replace == NULL)
       _replace = dm_empty_string;

    create_local_copy();

    index_type pos = 0;
    size_type cnt = 0;

    if(_len1==npos) _len1 = _dm_strlen(_seq);
    if(_len2==npos) _len2 = _dm_strlen(_replace);

    do {
      if(pos < _impl->Size() ) {
        pos = Find(_seq,pos);
        if(pos!=npos) {
          Replace(pos,_len1,_replace);
          ++cnt;
          pos += _len2;
        }
      } else pos  = npos;
    } while( pos != npos );
    return cnt;
  }
  return 0;
}
//------------------------------------------
dmTAString_CharT::index_type
dmTAString_CharT::ReplaceString( const dmTAString_CharT& _seq,const dmTAString_CharT& _replace )
{
  if(!_seq.IsEmpty())
   return ReplaceString(_seq.get(),_seq.Size(),_replace.get(),_replace.Size());

  return 0;
}
//------------------------------------------
dmTAString_CharT::index_type
dmTAString_CharT::Parse( dmTAString_CharT& _s, index_type pos, const char_type* _separators ) const
{
  if(_impl)
  {
    if(_separators == NULL)
      _separators = dm_white_spaces;

    if(pos != npos) {
      index_type _pos = FindFirstOf(_separators,pos);
      if(_pos == npos) {
        _s.Assign(*this,pos,_pos);
        return _pos;
      } else {
        _s.Assign(*this,pos,_pos-pos);
        return _pos+1;
      }
    } else _s.Clear();
  }
  return npos;
}
//------------------------------------------
dmTAString_CharT::index_type
dmTAString_CharT::Split(  dmTAString_CharT& _s, index_type pos, const char_type* _separators ) const
{
  _s.Clear();
  if(_impl)
  {
    if(_separators == NULL)
      _separators = dm_white_spaces;

    pos = FindFirstNotOf(_separators,pos);
    if(pos != npos) {
       index_type _end = FindFirstOf(_separators,pos);
       _s.Assign(*this,pos,_end - pos);
       pos = FindFirstNotOf(_separators,_end);
    }
    return pos;
  }
  return npos;
}
//-----------------------------------------------------------------------------
dmTAString_CharT& dmTAString_CharT::Format( size_type n, const char_type* fmt ... )
{
  create_local_copy();

  if(!is_null_string(fmt))
  {
    va_list ap;
    va_start(ap, fmt);
    dmTString_implementation::_Format( _impl, n, fmt, ap );
  }
  else
    Clear();

  return *this;
}
//------------------------------------------
dmTAString_CharT::char_type  dmTAString_CharT::operator[]( index_type p ) const
{
  return (*_impl)[p];
}
//------------------------------------------
dmTAString_CharT::char_type& dmTAString_CharT::operator[]( index_type p )
{
  create_local_copy();
  return (*_impl)[p];
}
//------------------------------------------
int dmTAString_CharT::Compare(const char_type *s, size_type n, bool _Case) const
{
  if(is_null_string(s))
    return IsEmpty() ? 0 : 1;
  else
   if(IsEmpty()) return -1;
  else
  {
    if(n==npos)
       n = _dm_strlen(s);

    if(_Case)
      return _dm_strncmp(Begin(),s,n);
    else
     return _dm_strncasecmp(Begin(),s,n);
  }
}
//------------------------------------------
dmTAString_CharT& dmTAString_CharT::ToLower()
{
  if(_impl)
  {
    create_local_copy();

    char_type* _First = _impl->Begin();
    char_type* _Last  = _impl->End();
    for(;_First!=_Last;++_First)
        *_First = _dm_tolower(*_First);
  }

  return *this;
}
//------------------------------------------
dmTAString_CharT& dmTAString_CharT::ToUpper()
{
  if(_impl)
  {
    create_local_copy();

    char_type* _First = _impl->Begin();
    char_type* _Last  = _impl->End();
    for(;_First!=_Last;++_First)
        *_First = _dm_toupper(*_First);
  }

  return *this;
}
//------------------------------------------
