#ifndef dmVector_h
#define dmVector_h

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
// File         : dmVector.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

//-------------------------------------------------------------
// This is (again !) a template vector implementation
// It's only purposes is to serve as a cross-library container 
// DO NOT USE IT !, use the stl implementation
// of your choice instead.
//-------------------------------------------------------------
template<class T, class A = dmAllocator<T> >
class dmVector
{
  public:
   typedef size_t size_type;
   typedef T *pointer;
   typedef const T *const_pointer;
   typedef T& reference;
   typedef const T& const_reference;
   typedef T value_type;
   typedef A allocator_type;
  
   typedef pointer       iterator;
   typedef const_pointer const_iterator;

  public:
   size_type Size()     const  { return size_type(_M_End - _M_Begin);  }
   size_type MaxSize()  const  { return size_type(-1) / sizeof(T); }
   size_type Capacity() const  { return size_type(_M_Last - _M_Begin); }
   bool      Empty()    const  { return _M_End == _M_Begin; }

   iterator  Begin() { return _M_Begin; }
   iterator  End()   { return _M_End;   }

   const_iterator  Begin() const { return _M_Begin; }
   const_iterator  End()   const { return _M_End;   }

   iterator Erase(iterator __position) {
     if (__position + 1 != End())
        _M_Copy(__position + 1,_M_End,__position);
     --_M_End;
     _Allocator.destroy(_M_End);
     return __position;
   }

   iterator Erase(iterator __first, iterator __last) {
     pointer __p = _M_Copy(__last,_M_End,__first);
     _M_Destroy(__p,_M_End);
     _M_End = __p;
     return __first;
   }

   iterator Insert(iterator __position, const_reference _R) {
     size_type __n = __position - Begin();
     if (_M_End != _M_Last) {
       if (__position == End()) {
         _Allocator.construct(_M_End,_R);
         ++_M_End;
       } else {
         _Allocator.construct(_M_End, *(_M_End - 1));
         ++_M_End;
         value_type __x_copy = _R;
         _M_Backward_Copy(__position,_M_End-2,_M_End-1);
         *__position = __x_copy;
       }
     }
     else
       _M_Insert_Overflow(__position,_R,1);

     return Begin() + __n;
   }

   iterator Insert(iterator __position,const_iterator _F, const_iterator _L ) {
     size_type __n        = __position - Begin();
     size_type __new_size = (_L - _F) + Size();
     if (Capacity() > __new_size) {
       if (__position == End())
         _M_End = _M_Uninitialized_Copy(_F,_L,__position);
       else
         _M_Insert_Overflow(__position,_F,_L);         
     }
     else
       _M_Insert_Overflow(__position,_F,_L);

     return Begin() + __n;

   }

   iterator Insert(iterator __position, size_type n, const_reference _R ) {
     size_type __n = __position - Begin();
     if(n>0) {        
       size_type __new_size =  n + Size();
       if (Capacity() > __new_size) {
         if (__position == End())
           _M_End = _M_Uninitialized_Fill(__position,__position+n,_R);
         else
           _M_Insert_Overflow(__position,_R,n);         
       }
       else
         _M_Insert_Overflow(__position,_R,n);
     }
     return Begin() + __n;
   }

   void Append( const_iterator _F, const_iterator _L ) {
     Insert(End(),_F,_L);
   }

   void Resize(size_type n, const_reference _R = value_type()) {
     if(n < Size()) 
       Erase(Begin() + n, End());
     else
       Insert(End(),n - Size(), _R);
   }

   void Clear() { 
     Erase(Begin(),End());
   }

   void Reserve(size_type n) {
     if(Capacity() < n) {
       size_type __old_size = Size();
       pointer __tmp;
       if(_M_Begin) {
         __tmp = _Allocator.allocate(n+1,NULL);
         _M_Uninitialized_Copy(_M_Begin,_M_End,__tmp);
         _M_Clear();
       } else {
         __tmp = _Allocator.allocate(n+1,NULL);
       }
       _M_Set(__tmp,__tmp+__old_size,__tmp+n);
     }
   }

   void Assign(  const_iterator _F, const_iterator _L ) {
     size_type __len = _L-_F;
     if (__len > Capacity()) {
       iterator __tmp = _Allocator.allocate(__len+1,NULL);
       _M_Clear();
       _M_Uninitialized_Copy(_F,_L,__tmp);
       _M_Set(__tmp,__tmp+__len,__tmp+__len);
     }
     else if (Size() >= __len) {
       iterator __new_finish = _M_Copy(_F,_L,_M_Begin);
       _M_Destroy(__new_finish,_M_End);
       _M_End = __new_finish;
     }
     else {
       const_iterator __mid = _F + Size() ;
       _M_Copy(_F, __mid,_M_Begin);
       _M_End = _M_Uninitialized_Copy(__mid,_L,_M_End);
     }
   }

   dmVector<T,A>& operator=( const dmVector<T,A>& _vec ) {
      if(&_vec != this )
        Assign(_vec.Begin(),_vec.End());
      return *this;
   }

   reference       operator[](size_type n)       { return *(Begin() + n); }
   const_reference operator[](size_type n) const { return *(Begin() + n); }

   reference        Front()       { return *Begin(); }
   const_reference  Front() const { return *Begin(); }
   reference        Back()        { return *(End() - 1); }
   const_reference  Back()  const { return *(End() - 1); }

   reference        At(size_type n)       { _M_CheckRange(n); return (*this)[n]; }
   const_reference  At(size_type n) const { _M_CheckRange(n); return (*this)[n]; }

   void Push_Back( const_reference _R ) {
     if (_M_End != _M_Last) {
       _Allocator.construct(_M_End,_R);
       ++_M_End;
     } else
       _M_Insert_Overflow(_M_End,_R,1);
   }

   void Pop_Back() {
     --_M_End;
     _Allocator.destroy(_M_End);
   }

   dmVector(const A& _A = A()) 
   : _M_Begin(0), _M_End(0), _M_Last(0), _Allocator(_A)  {}

   dmVector(size_type n, const_reference _R,const A& _A = A()) 
   : _M_Begin(0),_M_End(0),_M_Last(0),_Allocator(_A)  
   {
     if(n>0) { 
       _M_Begin = _Allocator.allocate(n+1,NULL);
       _M_End   = _M_Uninitialized_Fill(_M_Begin,_M_Begin+n,_R); 
       _M_Last  = _M_Begin + n;
     }
   }

   dmVector(size_type n,const A& _A = A())
   : _M_Begin(0),_M_End(0),_M_Last(0),_Allocator(_A)  
   {
     if(n>0) {   
       _M_Begin = _Allocator.allocate(n+1,NULL);
       _M_End   = _M_Begin; 
       _M_Last  = _M_Begin + n;
     }
   }

   dmVector(const dmVector<T,A>& _vec)
   : _M_Begin(0),_M_End(0),_M_Last(0),_Allocator(_vec._Allocator)
   { 
     size_t n = _vec.Size();
     if(n>0) {
       _M_Begin = _Allocator.allocate(n+1,NULL);
       _M_End   = _M_Uninitialized_Copy(_vec._M_Begin,_vec._M_End,_M_Begin);
       _M_Last  = _M_Begin + n;
     }
  }
 
   dmVector(const_iterator _F,const_iterator _L,const A& _A = A())
   : _M_Begin(0),_M_End(0),_M_Last(0),_Allocator(_A)
   {
     size_t n = _L - _F ;
     if(n>0) {
       _M_Begin = _Allocator.allocate(n+1,NULL);
       _M_End   = _M_Uninitialized_Copy(_F,_L,_M_Begin);
       _M_Last  = _M_Begin + n;
     }
   }


  ~dmVector() { _M_Clear(); }

  protected:
    void _M_Set( pointer __new_begin,pointer __new_end, pointer __new_last );
    void _M_Destroy(pointer _F,pointer _L);
    void _M_Clear();
    void _M_Insert_Overflow(pointer __position,const_reference _R,size_type __fill);
    void _M_Insert_Overflow(pointer __position,const_pointer _F,const_pointer _L);
    void _M_CheckRange(size_t) const;

    pointer _M_Uninitialized_Copy( const_pointer _F,const_pointer _L, pointer _I);
    pointer _M_Uninitialized_Fill( pointer _F,pointer _L, const_reference _R);
    pointer _M_Copy(const_pointer _F,const_pointer _L,pointer __position);
    pointer _M_Backward_Copy(const_pointer _F,const_pointer _L,pointer __position);
    pointer _M_Fill(pointer __position,const_reference _R,size_type __fill);

    pointer _M_Begin;
    pointer _M_End;
    pointer _M_Last;

    A _Allocator;
};

template<class T, class A>
inline void dmVector<T,A>::_M_Set( pointer __new_begin,pointer __new_end, pointer __new_last ) 
{
   _M_Begin = __new_begin;
   _M_End   = __new_end;
   _M_Last  = __new_last;
}

template<class T, class A>
void dmVector<T,A>::_M_Destroy(pointer _F,pointer _L) 
{
  for(;_F!=_L;++_F) 
    _Allocator.destroy(_F);
}

template<class T, class A>
void dmVector<T,A>::_M_Clear()
{
  if(_M_Begin!=_M_Last) {
    _M_Destroy(_M_Begin,_M_End);
    _Allocator.deallocate(_M_Begin,Capacity()+1);
  }
}

template<class T, class A>
T* dmVector<T,A>::_M_Uninitialized_Copy( const_pointer _F,const_pointer _L, pointer _I) {
  for(;_F!=_L;++_F) { 
     _Allocator.construct(_I,*_F); 
     ++_I;
  }
  return _I;
}

template<class T, class A>
T* dmVector<T,A>::_M_Uninitialized_Fill( pointer _F,pointer _L, const_reference _R) {
  for(;_F!=_L;++_F) {
    _Allocator.construct(_F,_R);
  }
  return _L;
}

template<class T, class A>
void dmVector<T,A>::_M_Insert_Overflow(pointer __position,const_reference _R,size_type __fill)
{
  const size_type __old_capacity = Capacity();
  const size_type __len          = __old_capacity + dm_max(__old_capacity,__fill);

  pointer __new_begin = _Allocator.allocate(__len+1,NULL);
  pointer __new_end   = __new_begin; 

  __new_end = _M_Uninitialized_Copy(_M_Begin,__position,__new_begin);
  __new_end = _M_Uninitialized_Fill(__new_end,__new_end+__fill,_R);
  __new_end = _M_Uninitialized_Copy(__position,_M_End,__new_end);

  _M_Clear();
  _M_Set(__new_begin,__new_end,__new_begin+__len);
}

template<class T, class A>
void dmVector<T,A>::_M_Insert_Overflow(pointer __position,const_pointer _F,const_pointer _L)
{
  const size_type __fill         = _L-_F;
  const size_type __old_capacity = Capacity();
  const size_type __len          = __old_capacity + dm_max(__old_capacity,__fill);

  pointer __new_begin = _Allocator.allocate(__len+1,NULL);
  pointer __new_end   = __new_begin; 

  __new_end = _M_Uninitialized_Copy(_M_Begin,__position,__new_begin);
  __new_end = _M_Uninitialized_Copy(_F,_L,__new_end);
  __new_end = _M_Uninitialized_Copy(__position,_M_End,__new_end);

  _M_Clear();
  _M_Set(__new_begin,__new_end,__new_begin+__len);
}

template<class T, class A>
inline T* dmVector<T,A>::_M_Copy(const_pointer _F,const_pointer _L,pointer __position)
{
  for(;_F!=_L;++_F) {
    *__position = *_F;
    ++__position;
  }
  return __position;
}

template<class T, class A>
inline T* dmVector<T,A>::_M_Fill(pointer __position,const_reference _R,size_type __fill)
{
  for(size_t n=0;n<__fill;++n) {
    *__position = _R;
    ++__position;
  }
  return __position;
}


template<class T, class A>
inline T* dmVector<T,A>::_M_Backward_Copy(const_pointer _F,const_pointer _L,pointer __position)
{
  while(_L!=_F)
     *--__position = *--_L;
  return __position;
}

template<class T, class A>
inline void dmVector<T,A>::_M_CheckRange(size_t n) const
{
  dmASSERT( n < Size() );    
}


#endif // dmVector.h
