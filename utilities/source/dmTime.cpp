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

#include "dmUtilitiesBase.h"
#include "dmString.h"
#include "dmDate.h"
#include "dmTime.h"

#include <time.h>

//-------------------------------------------------------------
#define SECONDS_PER_DAY 86400
//-------------------------------------------------------------
extern long __dm_dtojul( int  _day, int  _month, int  _year );
extern void __dm_jultod( long _julian, int&  _day, int&  _month, int&  _year );
//-------------------------------------------------------------
dmTime::dmTime()
{
  time_t ltime;
  time( &ltime ); // recupère la date système

  struct tm *_tm = localtime(&ltime);
  SetDate(_tm->tm_mday,_tm->tm_mon+1,_tm->tm_year+1900);
  SetTime(_tm->tm_hour,_tm->tm_min,_tm->tm_sec);
}
//-------------------------------------------------------------
dmTime::dmTime( int _day, int _month, int _year,
                        int _hour,int _min  , int _sec)
{
  SetDate(_day,_month,_year);
  SetTime(_hour,_min,_sec);
}
//-------------------------------------------------------------
dmTime::dmTime( const dmDate& d, int _hour,int _min, int _sec )
{
  SetDate(d);
  SetTime(_hour,_min,_sec);
}
//-------------------------------------------------------------
void dmTime::GetTime( int& _hour, int& _min, int& _sec ) const
{
  _hour = Hour();
  _min  = Minutes();
  _sec  = Seconds();
}
//-------------------------------------------------------------
void dmTime::SetTime( int _hour, int _min, int _sec )
{
  _value.nsecs  = 0;
  operator+=(_hour*3600 + _min*60 + _sec);
}
//-------------------------------------------------------------
void dmTime::GetDate( int& _day, int& _month, int& _year ) const
{
  __dm_jultod( _value.ndays,_day,_month,_year );
}
//-------------------------------------------------------------
void dmTime::SetDate( int _day, int _month, int _year )
{
  _value.ndays = __dm_dtojul( _day,_month,_year );
}
//-------------------------------------------------------------
dmDate dmTime::GetDate() const
{
  return _value.ndays;
}
//-------------------------------------------------------------
void dmTime::SetDate( const dmDate& d)
{
  _value.ndays = d.julian();
}
//-------------------------------------------------------------
int dmTime::Hour() const
{
  return (_value.nsecs / 3600);
}
//-------------------------------------------------------------
int dmTime::Minutes() const
{
  return (_value.nsecs % 3600) / 60;
}
//-------------------------------------------------------------
int dmTime::Seconds() const
{
  return ((_value.nsecs % 3600) % 60);
}
//-------------------------------------------------------------
dmTime& dmTime::operator++()
{
  if(++_value.nsecs == SECONDS_PER_DAY) {
    _value.nsecs = 0;
    ++_value.ndays;
  }
  return *this;
}
//-------------------------------------------------------------
dmTime& dmTime::operator--()
{
  if(--_value.nsecs < 0) 
  {
    _value.nsecs = SECONDS_PER_DAY-1;
    --_value.ndays;
  }
  return *this;
}
//-------------------------------------------------------------
dmTime dmTime::operator++(int)
{
  dmTime _tmp(*this);
  operator++();
  return _tmp;
}
//-------------------------------------------------------------
dmTime dmTime::operator--(int)
{
  dmTime _tmp(*this);
  operator--();
  return _tmp;
}
//-------------------------------------------------------------
dmTime& dmTime::operator+=( int _nsec )
{
  _value.nsecs += _nsec;
  if(_value.nsecs<0) {
    _value.nsecs  = - _value.nsecs;
    _value.ndays -= (1 + (_value.nsecs / SECONDS_PER_DAY));
    _value.nsecs  = SECONDS_PER_DAY - (_value.nsecs % SECONDS_PER_DAY); 
  } else if(_value.nsecs >= SECONDS_PER_DAY) {
    _value.ndays += _value.nsecs / SECONDS_PER_DAY;
    _value.nsecs  = _value.nsecs % SECONDS_PER_DAY; 
  }
  return *this;  
}
//-------------------------------------------------------------
dmTime& dmTime::operator-=( int _nsec )
{
  return operator+=( -_nsec );
}
//-------------------------------------------------------------
double dmTime::operator-( const dmTime& _t ) const
{
  long d_diff = _value.ndays - _t._value.ndays;
  long t_diff = _value.nsecs - _t._value.nsecs;
  return ((double)d_diff)*SECONDS_PER_DAY + t_diff;     
}
//-------------------------------------------------------------
dmTime dmTime::operator+( int _nsec )
{
  dmTime d(*this);
  d += _nsec;
  return d; 
}
//-------------------------------------------------------------
dmTime dmTime::operator-( int _nsec )
{
  return operator+( - _nsec );
}
//-------------------------------------------------------------
int dmTime::Compare(const dmTime& d) const 
{
  if(d._value.ndays == _value.ndays) {
    return _value.nsecs - d._value.nsecs;
  } else return _value.ndays - d._value.ndays;
}
//-------------------------------------------------------------
dmTime dmTime::Now()
{
  return dmTime();
}
//---------------------------------------------------------------
#define TOKEN_SEP ":"
#define TOKEN_CHR ':'
// construit une chaine de la forme "yyyy:mm:dd:hh:mm:ss"
dmCString dmTime::StringRep() const
{
  int _day,_month,_year;
  GetDate( _day, _month, _year );
  dmCString _rep;
  _rep.Format(dmCString::npos,"%04d%c%02d%c%02d%c%02d%c%02d%c%02d",
     _year,TOKEN_CHR,_month,TOKEN_CHR,_day,TOKEN_CHR,
     Hour(),TOKEN_CHR,Minutes(),TOKEN_CHR,Seconds());
  return _rep;
}
//---------------------------------------------------------------
// construit une date à partir de la representation retournée
// par string_rep()
//
void  dmTime::FromString( const dmCString& str_rep )
{
   dmCString res;
   int _day, _month, _year;
   size_t pos = 0;
   // Keep track of old représentation
   if(str_rep.FindFirstOf(TOKEN_SEP) < 4) {
     pos = str_rep.Parse( res, pos, TOKEN_SEP ); _day   = res.ToInt();
     pos = str_rep.Parse( res, pos, TOKEN_SEP ); _month = res.ToInt();
     pos = str_rep.Parse( res, pos, TOKEN_SEP ); _year  = res.ToInt();     
   } else {
     pos = str_rep.Parse( res, pos, TOKEN_SEP ); _year  = res.ToInt();
     pos = str_rep.Parse( res, pos, TOKEN_SEP ); _month = res.ToInt();
     pos = str_rep.Parse( res, pos, TOKEN_SEP ); _day   = res.ToInt();
   }
   SetDate( _day, _month, _year );
   if(pos != dmCString::npos) {
     int _hour,_min,_sec;
     pos = str_rep.Parse( res, pos, TOKEN_SEP ); _hour = res.ToInt();
     pos = str_rep.Parse( res, pos, TOKEN_SEP ); _min  = res.ToInt();
     pos = str_rep.Parse( res, pos, TOKEN_SEP ); _sec  = res.ToInt();
     SetTime(_hour,_min,_sec);
   } 
}

//---------------------------------------------------------------
dmCString dmTime::ToString( const dmCString::char_type* _fmt ) const
{
  dmCString res = GetDate().ToString(_fmt),buf;
  int r = 0;
  bool ok = false;
  for(size_t pos = 0;(pos=res.FindFirstOf('#',pos)) != dmCString::npos;) 
  {
    ++pos;
    switch( res[pos] ) {
      case 'h' : r = Hour()   ;ok=true; break;
      case 'm' : r = Minutes();ok=true; break;
      case 's' : r = Seconds();ok=true; break;
    }
    if(ok) { res.Replace(pos-1,2,buf.Format(3,"%d",r)); ok=false; }
  }
  return res;
}
//---------------------------------------------------------------
dmCString dmTime::ToString() const
{
  return ToString( "#D/#M/#Y #h h #m m" );
} 
//---------------------------------------------------------------

