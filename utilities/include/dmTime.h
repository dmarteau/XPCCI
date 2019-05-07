#ifndef dmTime_h
#define dmTime_h

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
// File         : dmTime.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

class __dmUtilities dmTime 
{
   public:
     typedef struct {
       long ndays;
       long nsecs;
     } time_type;
   
   private:
     time_type _value;

   public:
     dmTime();
     dmTime( time_type _t ) : _value(_t) {}
     dmTime( const dmTime& _aTime ) : _value(_aTime._value) {}
     dmTime( int _day, int _month, int _year,
             int _hour=0,int _min=0, int _sec =0);

     dmTime( const dmDate& , int _hour=0, int _min=0, int _sec =0 );

     dmTime& operator=( const dmTime& t ) { _value = t._value; return *this; }

    ~dmTime() {}

     void GetTime( int& _hour, int& _min, int& _sec ) const;
     void SetTime( int  _hour, int  _min, int  _sec );

     void GetDate( int& _day, int& _month, int& _year ) const;
     void SetDate( int  _day, int _month , int _year );

     dmDate GetDate() const;
     void   SetDate( const dmDate& );

     dmTime& operator++();
     dmTime& operator--();

     dmTime operator++(int);
     dmTime operator--(int);

     dmTime& operator+=( int _nsec );
     dmTime& operator-=( int _nsec );
     
     double operator-( const dmTime& _t ) const;

     dmTime operator+( int _nsec );
     dmTime operator-( int _nsec );

     int Hour()    const;
     int Minutes() const;
     int Seconds() const;

     // *Note: Algebrical comparison betwwen two times

     int Compare(const dmTime&) const;

     bool operator> ( const dmTime& t ) const { return Compare(t)> 0; }
     bool operator< ( const dmTime& t ) const { return Compare(t)< 0; } 
     bool operator<=( const dmTime& t ) const { return Compare(t)<=0; }
     bool operator>=( const dmTime& t ) const { return Compare(t)>=0; }
     bool operator==( const dmTime& t ) const { return Compare(t)==0; }
     bool operator!=( const dmTime& t ) const { return Compare(t)!=0; }

     bool IsOlderThan( const dmTime& t ) const { return Compare(t)<0; }

     static dmTime Now();

     // construit une chaine de la forme "dd:mm:yyyy:h:min:sec"
     dmCString StringRep() const;

     // construit une date à partir de la representation retournée
     // par StringRep)
     void  FromString( const dmCString& str_rep );

     // use #h = hour, #m = minutes, #s = seconds in _fmt
     dmCString ToString( const dmCString::char_type* _fmt ) const;
     dmCString ToString() const; // return default format string 

};

#endif // dmTime.h

