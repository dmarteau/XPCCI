#ifndef dmDate_h
#define dmDate_h

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
// File         : dmDate.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
enum {
  w_sunday = 0,
  w_monday,
  w_tuesday,
  w_thirsday,
  w_friday,
  w_saturday
};
/////////////////////////////////////////////////////////////////////////////
class __dmUtilities dmDate
{
   private:
     long _julian;

   public:
     dmDate();
     dmDate( long _jul ) : _julian(_jul) {}
     dmDate( const dmDate& _date ) : _julian(_date._julian) {}
     dmDate( int _day, int _month, int year );

     dmDate& operator=( const dmDate& d ) { _julian = d._julian; return *this; }

    ~dmDate() {}

     long julian() const { return _julian; }

     void GetDate( int& _day, int& _month, int& _year ) const;
     void SetDate( int  _day, int  _month, int  _year );

     int WeekDay() const; // O==sunday..7==monday

     dmDate& operator++() { ++_julian; return *this; }
     dmDate& operator--() { --_julian; return *this; }

     dmDate operator++(int) { long _jul = _julian++; return _jul; }
     dmDate operator--(int) { long _jul = _julian--; return _jul; }

     dmDate& operator+=( int _ndays )  { _julian+=_ndays; return *this; }
     dmDate& operator-=( int _ndays )  { _julian-=_ndays; return *this; }

     int operator-( const dmDate& _d ) const { return _julian - _d._julian; }

     dmDate operator+( int _ndays )  { return _julian+_ndays; }
     dmDate operator-( int _ndays )  { return _julian-_ndays; }

     // Build a string formatted as "dd:mm:yyyy"
     dmCString StringRep() const;

     // Build date from the representation returned by StringRep()
     void  FromString( const dmCString& str_rep );

     // use #D = day, #Y = year, #M = month in _fmt
     dmCString ToString( const dmCString::char_type* _fmt ) const;
     dmCString ToString() const;  // return default format string

     bool operator> ( const dmDate& d ) const { return _julian >  d._julian; }
     bool operator< ( const dmDate& d ) const { return _julian <  d._julian; }
     bool operator<=( const dmDate& d ) const { return _julian <= d._julian; }
     bool operator>=( const dmDate& d ) const { return _julian >= d._julian; }
     bool operator==( const dmDate& d ) const { return _julian == d._julian; }
     bool operator!=( const dmDate& d ) const { return _julian != d._julian; }

     static dmDate Today();

};


//////////////////////////////////////////////////////////////////////////////
#endif // dmDate_h

