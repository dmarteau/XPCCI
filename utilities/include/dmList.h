#ifndef dmList_h
#define dmList_h

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
// File         : dmList.h
// Date         : 6/2004
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//--------------------------------------------------------

//------------------------------------------------------------------------------
#ifndef NULL
#define NULL 0
#endif
//------------------------------------------------------------------------------
// class dmNode
//------------------------------------------------------------------------------
class dmList; // forward
class __dmUtilities dmNode
{
  friend class dmList;
  private:
    dmNode *ln_Succ;
    dmNode *ln_Pred;
    void Neutralize() { ln_Succ = ln_Pred = this; }

    dmNode( const dmNode& ) {}  // copie interdite

  protected :
    dmNode* Remove();  // Safe,
    void    Insert( dmNode* n );
 
  public:
    dmNode() { Neutralize(); }
    virtual ~dmNode() { Remove(); }
    //-----------------------------------
    dmNode* Next()   const { return ln_Succ;}
    dmNode* Prev()   const { return ln_Pred;}
    bool    Lonely() const { return Next()==this; }

};
//------------------------------------------------------------------------------
// class dmList
//------------------------------------------------------------------------------
class __dmUtilities dmList
{
  typedef dmNode* p_link;
  private:
     dmNode lh_Head;

  protected:
     p_link _Begin() const { return lh_Head.ln_Succ; }
     p_link _End()   const { return (p_link)&lh_Head; }
  
  public:
    //---- Definition d'un iterateur associe, type bidirectionels -----------------
    //class iterator;
    class iterator {
    friend class dmList;
      private:
        p_link node;
        iterator(p_link x) : node(x) {}
      public:
        iterator() : node(NULL) {}
        iterator( const iterator& it) : node(it.node) {}
        bool operator==(const iterator& x) const { return node == x.node; }
        bool operator!=(const iterator& x) const { return node != x.node; }

        p_link operator->() const { return node; }
        p_link operator*()  const { return node; }
        operator   p_link() const { return node; }

        iterator& operator++() { node = node->Next(); return *this; }
        iterator& operator--() { node = node->Prev(); return *this; }

        iterator operator++(int) { return (node=node->Next())->Prev(); }
        iterator operator--(int) { return (node=node->Prev())->Next(); }
    };

  public:
    iterator __Iterator( p_link x ) { return iterator(x); }

    // insere AVANT "position"
    iterator Insert(iterator position, p_link x) {
      if(!x->Lonely()) x->Remove();
      (x->ln_Pred = position->ln_Pred)->ln_Succ = x;
      (x->ln_Succ = position)->ln_Pred = x;
      return x;
    }

    iterator Begin() const { return _Begin(); }
    iterator End()   const { return _End();   }

    iterator First() const { return Begin(); }
    iterator Last()  const { return --End(); }

    p_link Head() const { return First(); }  
    p_link Tail() const { return Last();  }

    bool Empty() const { return lh_Head.Lonely(); }

    iterator Find( p_link x ) const {
       for(iterator it = Begin(); it!=End(); ++it ) {
         if(it.node==x) return it;
       }
       return End();
    }
    //--- Constructeurs --------------------------------------------------------
    dmList() {}
    virtual ~dmList() {}
    //--------------------------------------------------------------------------
  public:
    void Push_Front(p_link x) { Insert(Begin(),x); }
    void Push_Back (p_link x) { Insert(End(),x);   }

    p_link Remove(iterator position)  { return position->Remove(); }
    p_link Remove(p_link x          ) { return x->Remove(); }

    p_link Pop_Front() { return ( Empty() ? NULL : Remove(First()) ); }
    p_link Pop_Back()  { return ( Empty() ? NULL : Remove(Last())  ); }

    void Splice( iterator position, iterator it_start, iterator it_end );
    void Splice( iterator position, dmList& list ) { Splice(position,list.Begin(),list.End()); }

    //---------------------------------------------------------------------
    int  Count(iterator it_start, iterator it_end) const;
    int  Count() const { return Count(Begin(),End()); }

    size_t Size() const { return Count(); }

    void Clear() { lh_Head.Remove(); }
    //---------------------------------------------------------------------

};
//------------------------------------------------------------------------------
// Interface de Liste intrusive pour les noeuds typés
// La classe N doit hériter de dmNode
//------------------------------------------------------------------------------
template<class N>
class dmOList : protected dmList
{
 public:
   typedef dmOList<N> _Lt;
   typedef N _Nt;
   typedef N *pointer;
   typedef const N *const_pointer;
   typedef N& reference;
   typedef const N& const_reference;
   typedef N value_type;
  
   //static pointer _Cast( dmNode* n ) { return (pointer)n; }

 public:  
    //-----------------------------------------------------
    //class iterator;
    //-----------------------------------------------------
    class iterator
    {
      friend class dmOList<N>;
      private:
        _Nt* n;
        iterator( _Nt* x ) : n(x) {}         
      public: 

        _Nt* _Node() { return n; }

        iterator() : n(NULL) {}
        iterator( const iterator& it ) : n(it.n) {}

        bool operator==(const iterator& x) const { return n == x.n; }
        bool operator!=(const iterator& x) const { return n != x.n; }

        pointer operator->()  { return  n; }
        operator pointer()    { return  n; }
        reference operator*() { return *n; }

        iterator& operator++() { n = static_cast<_Nt*>(n->Next()); return *this; }
        iterator& operator--() { n = static_cast<_Nt*>(n->Prev()); return *this; }

        iterator operator++(int) { return (n=static_cast<_Nt*>(n->Next()))->Prev(); }
        iterator operator--(int) { return (n=static_cast<_Nt*>(n->Prev()))->Next(); }

        iterator Next() { return n->Next(); }
        iterator Prev() { return n->Prev(); }
 
        operator bool () const { return n != NULL; }
    };
    //-----------------------------------------------------
    //class const iterator;
    //-----------------------------------------------------
    class const_iterator
    {
      friend class dmOList<N>;
      private:
        _Nt* n;
      public:      
        const _Nt* _Node() { return n; }
       
        const_iterator() : n(NULL) {}
        const_iterator( const const_iterator& it) : n(it.n) {}
        const_iterator( const iterator& it)       : n(it.n) {}

        bool operator==(const const_iterator& x) const { return n == x.n; }
        bool operator!=(const const_iterator& x) const { return n != x.n; }

        const_pointer operator->()  const { return  n; }
        operator const_pointer()    const { return  n; }
        const_reference operator*() const { return *n; }

        const_iterator& operator++() { n = n->Next(); return *this; }
        const_iterator& operator--() { n = n->Prev(); return *this; }

        const_iterator operator++(int) { return (n=n->Next())->Prev(); }
        const_iterator operator--(int) { return (n=n->Prev())->Next(); }

        operator bool () const { return n != NULL; }
    };

  public:

    static iterator Iterator( _Nt* n ) { return iterator(n); } 

    iterator Begin() { return reinterpret_cast<_Nt*>(dmList::_Begin()); }
    iterator End()   { return reinterpret_cast<_Nt*>(dmList::_End());   }

    iterator First() { return Begin(); }
    iterator Last()  { return --End(); }
	  
    const_iterator Begin() const { return dmList::_Begin(); }
    const_iterator End()   const { return dmList::_End();   }

    const_iterator First() const { return Begin(); }
    const_iterator Last()  const { return --End(); }

    bool Empty() const { return dmList::Empty(); }

	  reference Head() { return *((pointer)dmList::Head()); }
	  reference Tail() { return *((pointer)dmList::Tail()); }

    const_reference Head() const { return *((pointer)dmList::Head()); }
    const_reference Tail() const { return *((pointer)dmList::Tail()); }

    iterator Insert( iterator pos,  pointer x )
    {
      dmList::Insert(__Iterator(pos._Node()),x);
      return pos.Prev();
  	}
 
    void Splice( iterator pos, iterator _start, iterator _end ) {
      dmList::Splice(pos,_start._Node(),_end._Node());
    }
	    
    void Splice( iterator pos, dmOList<N>& list ) { Insert(pos,list.Begin(),list.End()); }

    pointer Remove(iterator pos) { return (pointer)dmList::Remove(pos._Node()); }
	  
    void Push_Front( pointer x ) { dmList::Push_Front(x); }
    void Push_Back ( pointer x ) { dmList::Push_Back (x); }
	  
    void Push_Front( reference x ) { Push_Front(&x); }
    void Push_Back ( reference x ) { Push_Back (&x); }

    pointer Pop_Front() { return reinterpret_cast<pointer>(dmList::Pop_Front()); }
  	pointer Pop_Back () { return reinterpret_cast<pointer>(dmList::Pop_Back ()); }

    //---------------------------------------------
    dmOList() {}
    virtual ~dmOList() {}

    size_t Count() const { return dmList::Count(); }
    size_t Size()  const { return dmList::Size();  }

    void Clear() { dmList::Clear(); }

}; // class dmOList
//------------------------------------------------------------------------------
// Modèle de liste chainée : les noeuds peuvent s'organiser
// en liste circulaire
//------------------------------------------------------------------------------
template<class N> class dmOList; 
//------------------------------------------------------------------------------
template<class T> class dmTNode : protected dmNode
{
  typedef dmTNode<T>* p_link;

  friend class dmOList< dmTNode<T> >;

  public:
   typedef T *pointer;
   typedef const T *const_pointer;
   typedef T& reference;
   typedef const T& const_reference;
   typedef T value_type;

  private:
    value_type _elem;

  public:
    p_link Next() const { return reinterpret_cast<p_link>(dmNode::Next()); }
    p_link Prev() const { return reinterpret_cast<p_link>(dmNode::Prev()); }

    bool Lonely() const { return dmNode::Lonely(); }

    operator reference()             { return _elem; }
    operator const_reference() const { return _elem; }

    const_reference Elem() const { return _elem; }
    reference       Elem()       { return _elem; }

    reference Assign( const_reference x ) { return (_elem = x); }

    dmTNode( const_reference x ) : _elem(x) {}

    virtual ~dmTNode() {}

    // Pour eviter des warnings
    static p_link  _Cast( dmNode* p ) { return ( p_link )p; }
    static dmNode* _Node( p_link  p ) { return p;           }
   //------------------------------------------------
   class iterator
   {
      friend class const_iterator;
      private:
        p_link n;
      public:
        p_link _Node() { return n; }

        iterator() : n(0) {}
        iterator(p_link _n)           : n(_n)   {}
        iterator(const iterator& it ) : n(it.n) {}
        bool operator==(const iterator& x) const { return n == x.n; }
        bool operator!=(const iterator& x) const { return n != x.n; }

        reference operator*()  { return  n->Elem(); }
        pointer   operator->() { return &n->Elem(); }

        iterator& operator++() { n = n->Next(); return *this; }
        iterator& operator--() { n = n->Prev(); return *this; }

        iterator operator++(int) { return (n=n->next())->Prev(); }
        iterator operator--(int) { return (n=n->prev())->Next(); }

        bool operator!() const { return n==0; }
        operator bool()  const { return n!=0; }

        iterator Next() { return n->Next(); }
        iterator Prev() { return n->Prev(); }
    };

    class const_iterator
    {
      private:
        p_link n;
      public:
        p_link _Node() { return n; }

        const_iterator() : n(0) {}
        const_iterator(p_link _n)    : n(_n) {}
        const_iterator(const iterator& it) : n(it.n) {}
        bool operator==(const_iterator& x) const { return n == x.n; }
        bool operator!=(const_iterator& x) const { return n != x.n; }

        const_reference operator*()  const { return  n->Elem(); }
        const_pointer   operator->() const { return &n->Elem(); }
        
        const_pointer operator() () const { return n; }

        const_iterator& operator++() { n = n->next(); return *this; }
        const_iterator& operator--() { n = n->prev(); return *this; }

        const_iterator operator++(int) { return (n=n->Next())->Prev(); }
        const_iterator operator--(int) { return (n=n->Prev())->Next(); }

        bool operator!() const { return n==0; }
        operator bool()  const { return n!=0; }

        const_iterator Next() { return n->Next(); }
        const_iterator Prev() { return n->Prev(); }
    };
    //-------------------------------------------------------

    p_link Remove() { return (p_link)dmNode::Remove(); }
    
    // insert n before this
    void Insert(  p_link n ) { dmNode::Insert(n);  }

    void InsertBefore( p_link n ) { n->Insert(this); }
    void InsertAfter ( p_link n ) { Insert(n);       }

    void MoveBefore( p_link n ) { n->Insert( Remove() ); }
    void MoveAfter ( p_link n ) { Remove()->Insert(n);   }
    //----------------------------------------------------
    // Dans le cas de liste circulaire on insère une sentinelle 
    // avant l'iterator passé en paramètre 
    // Cela permet d'utiliser les operateurs du type _Op( _It first, _It last )
    //----------------------------------------------------
  public:
    struct sentry;
    friend struct sentry;
    struct sentry : protected dmNode
    {
      sentry( iterator it ) { Insert(it._Node()); }
     ~sentry() {} 
      iterator Begin() { return (p_link)Next(); }
      iterator End()   { return (p_link)this;}
    };

    struct const_sentry;
    friend struct const_sentry;
    struct const_sentry : protected dmNode
    {
      const_sentry( const_iterator it ) { Insert(it._Node()); }
     ~const_sentry() {} 
      const_iterator Begin() { return (p_link)Next(); }
      const_iterator End()   { return (p_link)this;   }
    };
};
//------------------------------------------------------------------------------
// Manager de Liste non intrusive
//------------------------------------------------------------------------------
template<class T, class A = dmFastAllocator< dmTNode<T> > >
class dmTList : protected dmList
{
 public:
   typedef dmTList<T,A>  _Lt;
   typedef dmTNode<T>    _Nt;
   typedef size_t size_type;
   typedef T *pointer;
   typedef const T *const_pointer;
   typedef T& reference;
   typedef const T& const_reference;
   typedef T value_type;
   typedef A allocator_type;
    
  private:
   A _allocator;
   typedef _Nt* p_link;

   p_link _getNode(const_reference value) {  
     p_link n =  _allocator.allocate(1,(_Nt*)0 );
     _allocator.construct(n,value);
     return n;
   }
	
   void _putNode(p_link node) { 
     _allocator.destroy(node);
     _allocator.deallocate(node,1);
   }

  public:
   //------------------------------------------------
   class const_iterator;
   class iterator
   {
      friend class const_iterator;
      private:
        p_link n;
      public:
        p_link _Node() { return n; }

        iterator() : n(0) {}
        iterator(p_link _n)           : n(_n)   {}
        iterator(const iterator& it ) : n(it.n) {}
        bool operator==(const iterator& x) const { return n == x.n; }
        bool operator!=(const iterator& x) const { return n != x.n; }

        reference operator*()  { return  n->Elem(); }
        pointer   operator->() { return &n->Elem(); }

        iterator& operator++() { n = n->Next(); return *this; }
        iterator& operator--() { n = n->Prev(); return *this; }

        iterator operator++(int) { return (n=n->Next())->Prev(); }
        iterator operator--(int) { return (n=n->Prev())->Next(); }

        bool operator!() const { return n==0; }
        operator bool()  const { return n!=0; }

        iterator Next() { return n->Next(); }
        iterator Prev() { return n->Prev(); }
    };

    class const_iterator
    {
      private:
        p_link n;
      public:
        p_link _Node() { return n; }

        const_iterator() : n(0) {}
        const_iterator(p_link _n)    : n(_n) {}
        const_iterator(const iterator& it) : n(it.n) {}
        bool operator==(const_iterator& x) const { return n == x.n; }
        bool operator!=(const_iterator& x) const { return n != x.n; }

        const_reference operator*()  const { return n->Elem(); }
        const_pointer   operator->() const { return &n->Elem(); }
 
        const_iterator& operator++() { n = n->Next(); return *this; }
        const_iterator& operator--() { n = n->Prev(); return *this; }

        const_iterator operator++(int) { return (n=n->Next())->Prev(); }
        const_iterator operator--(int) { return (n=n->Prev())->Next(); }

        bool operator!() const { return n==0; }
        operator bool()  const { return n!=0; }

        const_iterator Next() { return n->Next(); }
        const_iterator Prev() { return n->Prev(); }
    };
    //-------------------------------------------------------
    class const_reverse_iterator;
    class reverse_iterator
    {
      friend class const_reverse_iterator;
      private:
        p_link n;
      public:
        p_link _Node() { return n; }

        reverse_iterator() : n(0) {}
        reverse_iterator(p_link _n)           : n(_n)   {}
        reverse_iterator(const reverse_iterator& it ) : n(it.n) {}
        bool operator==(const reverse_iterator& x) const { return n == x.n; }
        bool operator!=(const reverse_iterator& x) const { return n != x.n; }

        reference operator*()  { return  n->Elem(); }
        pointer   operator->() { return &n->Elem(); }

        reverse_iterator& operator++() { n = n->Prev(); return *this; }
        reverse_iterator& operator--() { n = n->Next(); return *this; }

        reverse_iterator operator++(int) { return (n=n->Prev())->Next(); }
        reverse_iterator operator--(int) { return (n=n->Next())->Prev(); }

        bool operator!() const { return n==0; }
        operator bool()  const { return n!=0; }

        reverse_iterator Next() { return n->Prev(); }
        reverse_iterator Prev() { return n->Next(); }
    };

    class const_reverse_iterator
    {
      private:
        p_link n;
      public:
        p_link _Node() { return n; }

        const_reverse_iterator() : n(0) {}
        const_reverse_iterator(p_link _n)    : n(_n) {}
        const_reverse_iterator(const reverse_iterator& it) : n(it.n) {}
        bool operator==(const_reverse_iterator& x) const { return n == x.n; }
        bool operator!=(const_reverse_iterator& x) const { return n != x.n; }

        const_reference operator*()  const { return n->Elem(); }
        const_pointer   operator->() const { return &n->Eelem(); }

        const_reverse_iterator& operator++() { n = n->Prev(); return *this; }
        const_reverse_iterator& operator--() { n = n->Next(); return *this; }

        const_reverse_iterator operator++(int) { return (n=n->Prev())->Next(); }
        const_reverse_iterator operator--(int) { return (n=n->Next())->Prev(); }

        bool operator!() const { return n==0; }
        operator bool()  const { return n!=0; }

        const_reverse_iterator Next() { return n->Prev(); }
        const_reverse_iterator Prev() { return n->Next(); }
    };
    //-------------------------------------------------------
  public:
    iterator Begin() const { return _Nt::_Cast( _Begin() ); }
    iterator End()   const { return _Nt::_Cast( _End()   ); }

    iterator First() const { return Begin(); }
    iterator Last()  const { return --End(); }

    reverse_iterator rBegin() const { return _Nt::_Cast( _End()->Prev()   ); }
    reverse_iterator rEnd()   const { return _Nt::_Cast( _End() );           }

    bool Empty() const { return dmList::Empty(); }

    reference Head() { return _Nt::_Cast( dmList::Head() )->Elem(); }
    reference Tail() { return _Nt::_Cast( dmList::Tail() )->Elem(); }

    const_reference Head() const { return _Nt::_Cast( dmList::Head() )->Elem(); }
    const_reference Tail() const { return _Nt::_Cast( dmList::Tail() )->Elem(); }

    iterator Insert( iterator pos, const_reference value = T() ) {
      pos._Node()->Insert(_getNode(value));
      return pos.Prev();
  	}
	
    void Insert( iterator pos, int n, const_reference value = T() ) {
	    for(;--n>=0;) (void)Insert(pos,value);
	  }
    
    void Splice( iterator pos, _Lt& alist )                     { dmList::Splice(__Iterator(_Nt::_Node(pos._Node())),alist);       }
    void Splice( iterator pos, iterator _start, iterator _end ) { dmList::Splice(__Iterator(_Nt::_Node(pos._Node())),_start,_end); }

    void Insert( iterator pos, iterator _start, iterator _end ) {
      for(iterator it = _start;it!=_end;++it) 
      (void)Insert(pos,*it);
    }

    void Insert( iterator pos, const _Lt& list ) { Insert(pos,list.Begin(),list.End()); }


    void      Remove(iterator pos) { _putNode( _Nt::_Cast(dmList::Remove( _Nt::_Node(pos._Node())) ) ); }
    iterator  Erase (iterator pos) {
       iterator _next = pos.Next();
       Remove(pos);
       return _next;
    }

    iterator Find( const_reference _ref ) {
      iterator _F = Begin(); 
      iterator _L = End(); 
      for(;_F!=_L;++_F)
        if( *_F == _ref)
           return _F; 

      return End();
    }

    void Remove( const_reference _ref ) {
      iterator it = Find(_ref); 
      if(it!=End())
        Remove(it);
    }

    void Push_Front( const_reference value )  { dmList::Push_Front( _Nt::_Node(_getNode(value)) );  }
    void Push_Back ( const_reference value )  { dmList::Push_Back ( _Nt::_Node(_getNode(value)) );  }

    void Pop_Front() { _putNode( _Nt::_Cast( dmList::Pop_Front() ) ); }
  	void Pop_Back () { _putNode( _Nt::_Cast( dmList::Pop_Back () ) ); }

    void Destroy()         { while(!Empty()) Pop_Back();  }
    void Destroy_reverse() { while(!Empty()) Pop_Front(); }

    void   Clear() { Destroy(); }
    size_t Size() const { return dmList::Size(); }
	  //---------------------------------------------
    dmTList( const A& _A=A() ) : _allocator(_A) {}
    dmTList( const _Lt& list ) : _allocator(list._allocator) { Insert(End(),list); }
    dmTList& operator=( const _Lt& list ) {
      if(&list!=this) { 
        Destroy();
        Insert(End(),list); 
      }
      return *this;
    }

    virtual ~dmTList() { Destroy(); }
    const A& allocator() const { return _allocator; }

    size_t Count() const { return dmList::Count(); }

}; // class dmTList
//------------------------------------------------------------------------------
// Manager pour les listes circulaires
//------------------------------------------------------------------------------
template<class T, class A = dmFastAllocator< dmTNode<T> > >
class dmTCList
{
 public:
   typedef dmTCList<T,A> _Lt;
   typedef dmTNode<T>    _Nt;
   typedef size_t size_type;
   typedef T *pointer;
   typedef const T *const_pointer;
   typedef T& reference;
   typedef const T& const_reference;
   typedef T value_type;
   typedef A allocator_type;
   
   typedef typename _Nt::iterator iterator;
   typedef typename _Nt::const_iterator const_iterator;

  private:
    A _allocator;
    size_t _Size;
    _Nt*   _Base;
  
  protected:
    _Nt* _getNode( const T& X );
    void _putNode(_Nt* n);

  public:
    dmTCList( const A& _A=A() ) : _allocator(_A), _Base(0),_Size(0) {}
    dmTCList( const T& X, const A& _A=A() ) : _allocator(_A),_Size(0) { _getNode(X); }
   ~dmTCList() { Clear(); }

    operator iterator()             { return _Base; }
    operator const_iterator() const { return _Base; }

    void Pop() { _putNode(_Base); } 
    void Push( const_reference X ) {
       _Nt* n = _getNode(X);
       _Base->insert(n);
       _Base = n;
    }
   
    void Forward()  { if(_Base) _Base = _Base->Next(); }
    void Backward() { if(_Base) _Base = _Base->Prev(); }
    void Move( iterator it ) { _Base = it._Node();     }
    void Rotate( int n ) { 
       if(n>0) while(--n) Forward();
       else    while(++n) Backward();   
    }

    bool   Empty() const { return _Size==0; }
    void   Clear() { while(!Empty()) Pop(); }
    void   Remove( iterator it ) { _putNode(it._Node()); }
    size_t Size() const { return _Size; }

    void Insert( iterator it, const_reference X ) { it._Node()->insert( _getNode(X) ); }

    const A& allocator() const { return _allocator; }

    //-------------------------------------------------
};
//-----------------------------------------------------
template<class T,class A>
typename dmTCList<T,A>::_Nt* dmTCList<T,A>::_getNode( const T& X ) 
{
  _Nt* n =  _allocator.allocate(1,(_Nt*)0 );
  _allocator.construct(n,X);
  if(!_Base) _Base = n;
  ++_Size;
  return n;
}
//-----------------------------------------------------
template<class T,class A>
void dmTCList<T,A>::_putNode(typename dmTCList<T,A>::_Nt* n)
{ 
  if(--_Size==0) _Base=NULL;
  else if(n==_Base) { _Base = n->next(); }
  _allocator.destroy(n);
  _allocator.deallocate(n,1);
}
//------------------------------------------------------------------------------
#endif // piListH

