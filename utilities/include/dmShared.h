#ifndef dmShared_h
#define dmShared_h

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


//--------------------------------------------------------------------------
// reference counter class
//--------------------------------------------------------------------------
class dmReferenced
{
  private:
    int refCount;
  public:
    dmReferenced() : refCount(0) {}
    virtual ~dmReferenced() {}

    virtual int Obtain()  { return ++refCount; }
    virtual int Release() { return --refCount; }

    int  References()    const { return refCount;   }
    bool IsReferenced()  const { return refCount>0; }
};
//--------------------------------------------------------------------------
// Objects that are accessed from various other objects that should span the
// lifetime of their accessors can be tracked with the Shared class. The first
// accessing object creates the Shared instance, other objects may participate
// in the access and the last accessing object that releases its hold on the
// Shared instance causes its destruction.
//
// REMEMBER: Shared objects may only be constructed dynamically via the
//            new operator!
class dmShared : public dmReferenced
{
  public:
	dmShared() { dmReferenced::Obtain(); }  // construction implies an access permission.
 ~dmShared() {};

	int Release() { return (dmReferenced::Release()<=0 ? (delete this,0) : References()); }
	
	DM_DECL_ARENA_OPERATOR_NEW(dmShared)
};
//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
// Shared link sur une instance d'objet
// la double indirection est nescessaire si on veut pouvoir profiter
// du polymorphisme
//-------------------------------------------------------------------------
template<class T> class dmNewLink;

template<class T>  class dmLink
{
  friend class dmNewLink<T>;
  public:
    class p_handle : public dmShared {
      private:
        T*  ptr;
      public:
        T* Pointer() const { return ptr; }
        p_handle( T* p ) : ptr(p) {}
       ~p_handle() {}
    };

    p_handle* GetHandle() const { handle->Obtain(); return handle; }

  private:
    p_handle* handle;

  protected:
    explicit dmLink( p_handle* _handle ) : handle(_handle) {}

  public:
    typedef T value_type;

    bool IsNull()    const { return handle->Pointer() == NULL; }
    bool operator!() const { return IsNull(); }

    bool operator==(const dmLink<T>& _link) const { return handle == _link.handle; }
    bool operator!=(const dmLink<T>& _link) const { return !(*this==_link); }

    bool compare( const T* _ptr ) const { return handle->Pointer()==_ptr; }

    dmLink<T>& operator=( const dmLink<T>& _link ) {
      if(handle != _link.handle) {
        handle->Release();
        handle = _link.GetHandle();
      }
      return *this;
    }

    T* Pointer()    { return handle->Pointer(); }
    operator T*()   { return  Pointer(); }
    T& operator*()  { dmASSERT(!IsNull()); return *Pointer(); }
    T* operator->() { dmASSERT(!IsNull()); return  Pointer(); }

    dmLink() : handle(new p_handle(NULL)) {}
    dmLink( const dmLink<T>& _link ) : handle(_link.GetHandle()) {}
   ~dmLink() { handle->Release(); }

    // const version for operators
    const T* Pointer()    const { return handle->Pointer(); }
    operator const T*()   const { return  Pointer(); }
    const T& operator*()  const { dmASSERT(!IsNull()); return *Pointer(); }
    const T* operator->() const { dmASSERT(!IsNull()); return  Pointer(); }

    void Release() {
       handle->Release();
       handle = new p_handle(NULL);
    }

    int RefCount() const { return handle->References(); }

    bool IsAssigned() const {
      return (handle->Pointer()!=NULL||handle->References()>1);
    }

    T** BeginAssignment() {
       if(IsAssigned())
          Release();
       return &handle->Pointer();
    }

  protected:
    class getter
    {
      public:
        explicit getter( dmLink<T>& aLink )
        : __link(aLink)
        {}

        operator T**()  { return __link.BeginAssignment(); }
        T*& operator*() { return *(__link.BeginAssignment()); }

      private:
        dmLink<T>& __link;
    };
};

template<class T> typename dmLink<T>::getter
inline dmGetter_AddRefs( dmLink<T>& aLink ) {
  return dmLink<T>::getter(aLink);
}

//--------------------------------------------------------------------------
// const link => ensure const
//--------------------------------------------------------------------------
template<class T> class dmConstLink
{
  friend class dmNewLink<T>;
  private:
    dmLink<T> __link;
  public:
    typedef T value_type;

    bool IsNull()    const { return __link.IsNull(); }
    bool operator!() const { return IsNull(); }

    int RefCount()  const { return __link->RefCount(); }

    const T* Pointer()    const { return __link.Pointer(); }
    operator const T*()   const { return __link.Pointer(); }
    const T& operator*()  const { return __link.operator*();  }
    const T* operator->() const { return __link.operator->(); }

    dmConstLink<T>& operator=( const dmConstLink<T>& _clink) { __link =_clink.__link; return *this; }

    dmConstLink() {}
    dmConstLink( const dmLink<T>& _link )       : __link(_link) {}
    dmConstLink( const dmConstLink<T>& _clink ) : __link(_clink.__link) {}

    void Release() { __link.Release(); }
    bool Compare( const T* _ptr ) { return __link.Compare(_ptr); }

    bool operator==(const dmLink<T>& _link) const { return _link == __link; }
  	bool operator!=(const dmLink<T>& _link) const { return _link != __link; }

    bool operator==(const dmConstLink<T>& _link) const { return _link.__link == __link; }
    bool operator!=(const dmConstLink<T>& _link) const { return _link.__link != __link; }

   ~dmConstLink() {}
};
//--------------------------------------------------------------------------
template<class T>
class dmNewLink
{
  typedef typename dmLink<T>::p_handle handle_type;
  public:
    dmLink<T> self;
    explicit dmNewLink( T* p ) : self(new handle_type(p)) {}
    operator dmLink<T>& ()       { return self; }

    dmLink<T>& Link()  { return self; }
};
//--------------------------------------------------------------------------
template<class T>
inline dmLink<T> dmReturnLink(  T* p )
{
  dmNewLink<T> _aNewLink(p);
  return _aNewLink.Link();
}

//--------------------------------------------------------------------------
// auto link : handle auto deletion of the assigned pointer
//--------------------------------------------------------------------------
template<class T>
class dmAutoLink : public dmLink<T>
{
  public:
   class auto_handle : public dmLink<T>::p_handle {
     protected:
       void DeletePtr() {
         T* _Ptr = this->Pointer();
         if(_Ptr) delete _Ptr;
       }
     public:
       auto_handle( T* p ) : dmLink<T>::p_handle(p) {}
      ~auto_handle() { DeletePtr(); }
   };
  private:
   explicit dmAutoLink( dmLink<T>& ) {}
  public:
    explicit dmAutoLink( T* p ) : dmLink<T>(new  auto_handle(p)) {}
    dmAutoLink()  : dmLink<T>(new  auto_handle(NULL)) {}
   ~dmAutoLink() {}
};
//--------------------------------------------------------------------------
#endif //dmShared_h
