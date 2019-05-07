#ifndef dmSetOf_h
#define dmSetOf_h

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

//----------------------------------------------------------
// Cette class permet de manipuler des types enumeres de manière ensembliste
// Les enums ne doivent pas définir une valeur > 31
// la fonction via la macro DEFINE_SET_OF
// ex :
//
//   typdef enum {
//     element1,  (=0)
//     element2,  (=1)
//     ...
//     elementn   (=n-1)
//     no_element -> on peut se servir d'une balise qui retourne le nombre d'élément
//   } monType;
//
//   DEFINE_SET_OF( mon_Enum, no_element );
//
// usage :
// SetOf<monType> a_other_set = SetOf<monType>::all();
//
//----------------------------------------------------------

template<class T> class dmSetOf
{
  private :
    unsigned long values;

  protected:
    dmSetOf(unsigned long _values) : values(_values) {}

  public:
    dmSetOf()    : values(0) {}
    dmSetOf(T v) : values(1L << v) {}
    dmSetOf(const dmSetOf<T>& s) : values(s.values) {}

    virtual ~dmSetOf() {}

    void Assign( unsigned long x ) { values = x; }

    static inline dmSetOf<T> All();

    bool operator==(const dmSetOf& s) const { return s.values == values;  }
    bool operator!=(const dmSetOf& s) const { return !( *this == s ); }

    dmSetOf& operator=(const dmSetOf<T>& s) { values = s.values; return *this; }

    dmSetOf& operator+=(const dmSetOf& s) { values |=  s.values;  return *this; }
    dmSetOf& operator-=(const dmSetOf& s) { values &= ~s.values;  return *this; }
    dmSetOf& operator&=(const dmSetOf& s) { values &=  s.values;  return *this; }

    dmSetOf operator+(const dmSetOf& s) const { return values |  s.values; }
    dmSetOf operator-(const dmSetOf& s) const { return values & ~s.values; }
    dmSetOf operator&(const dmSetOf& s) const { return values &  s.values; }

    bool Contains( const dmSetOf<T>& s ) const { return  (*this & s) == s; }
    bool Empty()    const { return values != 0; }
    bool Complete() const { return *this == All(); }

    void Clear()  { values = 0; }

    // set values under boolean condition
    void Set( const dmSetOf<T>& s, bool _st ) { (_st?values|=s.values:values&=~s.values); }
};

//----------------------------------------------------------
#define dmDefineSetOf( name, size ) \
template<> inline  dmSetOf<name> dmSetOf<name>::All() { return (unsigned int)pow(2,size)-1 ; } \
// DEFINE_SET_OF
//----------------------------------------------------------


#endif // SetOfH
