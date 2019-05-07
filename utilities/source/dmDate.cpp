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


#include "dm_utilities.h"

#include <math.h>
#include <time.h>
//----------------------------------------------------------------

#define IGREG (15+31L*(10+12L*1582))
//----------------------------------------------------------------
long __dm_dtojul( int  _day, int  _month, int  _year )
{
  int ja,jy=_year,jm;
  //piASSERT( jy!=0 );
  if(jy<0) ++jy;
  if(_month > 2 ) { jm = _month+1; }
  else { --jy; jm=_month+13; }

  long _julian = (long) (floor(365.25*jy)+floor(30.6001*jm)+_day+1720995);
  if( _day+31L*(_month+12L*_year) >= IGREG ) {
    ja=(int)(0.01*jy);
    _julian += 2-ja+(int)(0.25*ja);
  }
  return _julian;
}
//----------------------------------------------------------------
void __dm_jultod( long _julian, int&  _day, int&  _month, int&  _year )
{
  long ja = _julian;
  if(_julian >= IGREG ) {
    long jalpha = (long)(((float) (_julian-1867216)-0.25)/36524.65);
    ja = _julian+1+jalpha-(long)(0.25*jalpha);
  } else if(_julian < 0) { ja = _julian +36525*(1-_julian/36525); }

  long jb = ja+1524;
  long jc = (long)(6680.0+((float)( jb-2439870 )-122.1)/365.25);
  long jd = (long)(365*jc+(0.25*jc));
  long je = (long)((jb-jd)/30.6001);
  _day   = (jb-jd)-(long)(30.6001*je);
  _month = je-1;
  if(_month>12) _month -= 12;
  _year = jc-4715;
  if(_month > 2 ) --_year;
  if(_year <= 0 ) --_year;
  if(_julian < 0) _year -= 100*(1-_julian/36525);
}
//-----------------------------------------------------------------
dmDate::dmDate()
{
  time_t ltime;
  time( &ltime ); // recup�re la date syst�me

  struct tm *_tm = localtime(&ltime);
  SetDate(_tm->tm_mday,_tm->tm_mon+1,_tm->tm_year+1900);
}
//-----------------------------------------------------------------
dmDate::dmDate( int _day, int _month, int _year )
{
  SetDate(_day,_month,_year);
}
//-----------------------------------------------------------------
void dmDate::GetDate( int& _day, int& _month, int& _year ) const
{
  __dm_jultod(_julian,_day,_month,_year);
}
//---------------------------------------------------------------
void dmDate::SetDate( int  _day, int  _month, int  _year )
{
  _julian = __dm_dtojul(_day,_month,_year);
}
//---------------------------------------------------------------
int dmDate::WeekDay() const { return ((_julian+1) % 7); }
//---------------------------------------------------------------
dmDate dmDate::Today()
{
  return dmDate();
}
//---------------------------------------------------------------
#define TOKEN_SEP ":"
#define TOKEN_CHR ':'

// construit une chaine de la forme "yyyy:mm:dd"
dmCString dmDate::StringRep() const
{
  int _day,_month,_year;
  GetDate( _day, _month, _year );
  dmCString _rep;
  _rep.Format(dmCString::npos,"%04d%c%02d%c%02d",_year,TOKEN_CHR,_month,TOKEN_CHR,_day);
  return _rep;
}
//---------------------------------------------------------------
// construit une date � partir de la representation retourn�e
// par StringRep()
//
void  dmDate::FromString( const dmCString& str_rep )
{
   dmCString res;
   int _day, _month, _year;
   size_t pos = 0;
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
}
//---------------------------------------------------------------
dmCString dmDate::ToString( const dmCString::char_type* _fmt ) const
{
  int _day,_month,_year,r = 0;
  bool ok = false;
  GetDate( _day, _month, _year );
  dmCString res(_fmt),buf;
  for(size_t pos = 0;(pos=res.FindFirstOf('#',pos)) != dmCString::npos;)
  {
    ++pos;
    switch( res[pos] ) {
      case 'D' : r = _day  ; ok = true; break;
      case 'Y' : r = _year ; ok = true; break;
      case 'M' : r = _month; ok = true; break;
    }
    if(ok) { res.Replace(pos-1,2,buf.Format(10,"%d",r)); ok=false; }
  }
  return res;
}
//---------------------------------------------------------------
dmCString dmDate::ToString() const
{
  static const char* __default = "#D/#M/#Y";
  return ToString(__default);
}
//---------------------------------------------------------------
