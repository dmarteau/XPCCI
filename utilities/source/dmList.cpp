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

#define NO_STL 
#include "dmFastMemory.h"
#include "dmList.h"

//---------------------------------------------------------------------
// class dmNode
//---------------------------------------------------------------------
dmNode* dmNode::Remove() 
{  // Safe,
  ln_Pred->ln_Succ = ln_Succ;
  ln_Succ->ln_Pred = ln_Pred;
  Neutralize();
  return this;
}
//---------------------------------------------------------------------
void dmNode::Insert( dmNode* n ) 
{
  (n->ln_Succ->ln_Pred = ln_Pred)->ln_Succ = n->ln_Succ;
  (n->ln_Succ = this)->ln_Pred = n;
}
//---------------------------------------------------------------------
// class dmList
//---------------------------------------------------------------------
int dmList::Count(iterator it_start, iterator it_end) const
{
  int n = 0;
  for(;it_start!=it_end;++it_start) 
     ++n;
  return n;
}
//---------------------------------------------------------------------
void dmList::Splice( iterator pos, iterator _start, iterator _end )
{
  if(_start!=_end)
  {
    p_link p_tail = _end->ln_Pred;
    (_end->ln_Pred   = _start->ln_Pred)->ln_Succ = _end;
    (_start->ln_Pred = pos->ln_Pred)->ln_Succ   =  _start;
    (pos->ln_Pred = p_tail)->ln_Succ = pos;
  }
}
//-----------------------------------------------------
