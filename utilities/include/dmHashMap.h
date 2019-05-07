#ifndef dmHashMap_h
#define dmHashMap_h

#include "dmHashtable.h"
#include "dmHashfuncs.h"
#include "dmFastMemory.h"

namespace daim {

template <class _Key, class _Val,class _HashFcn = daim::hash<_Key>,
          class _EqualKey = std::equal_to<_Key> > 

  class hashmap;

template <class _Key, class _Val, class _HashFn, class _EqKey>
inline bool operator==(const hashmap<_Key, _Val, _HashFn, _EqKey>&,
                       const hashmap<_Key, _Val, _HashFn, _EqKey>&);

//--------------------------------------------------------------
// hashmap
//--------------------------------------------------------------
template <class _Key, class _Val, class _HashFcn, class _EqualKey>
class hashmap
{
private:
  typedef std::pair<const _Key,_Val> _Tp;
  typedef _Hashtable_node<_Tp>  _Nt;

  typedef hashtable<_Tp,_Key,_HashFcn,_Select1st<_Tp>,_EqualKey,dmFastAllocator<_Nt> > _Ht;
  _Ht _M_ht;

public:
  typedef typename _Ht::key_type key_type;
  typedef typename _Ht::value_type value_type;
  typedef typename _Ht::hasher hasher;
  typedef typename _Ht::key_equal key_equal;
  
  typedef typename _Ht::size_type size_type;
  typedef typename _Ht::difference_type difference_type;
  typedef typename _Ht::pointer pointer;
  typedef typename _Ht::const_pointer const_pointer;
  typedef typename _Ht::reference reference;
  typedef typename _Ht::const_reference const_reference;
  typedef typename _Ht::iterator iterator;
  typedef typename _Ht::const_iterator const_iterator;
  typedef typename _Ht::allocator_type allocator_type;

  hasher    hash_funct()         const { return _M_ht.hash_funct(); }
  key_equal key_eq()             const { return _M_ht.key_eq(); }
  allocator_type get_allocator() const { return _M_ht.get_allocator(); }

public:
  hashmap(const allocator_type& __a = allocator_type())
  : _M_ht(100, hasher(), key_equal(),__a) {}

  explicit hashmap(size_type __n, const allocator_type& __a = allocator_type())
  : _M_ht(__n,hasher(),key_equal(),__a) {}
  
  hashmap(size_type __n, const hasher& __hf,const allocator_type& __a = allocator_type())
 : _M_ht(__n, __hf, key_equal(),__a) {}

  hashmap(size_type __n, const hasher& __hf, const key_equal& __eql,
          const allocator_type& __a = allocator_type())
  : _M_ht(__n, __hf, __eql, __a) {}

  template <class _InputIterator>
  hashmap(_InputIterator __f, _InputIterator __l)
    : _M_ht(100, hasher(), key_equal())
    { _M_ht.insert_unique(__f, __l); }
  
  template <class _InputIterator>
  hashmap(_InputIterator __f, _InputIterator __l, size_type __n)
    : _M_ht(__n, hasher(), key_equal())
    { _M_ht.insert_unique(__f, __l); }
  
  template <class _InputIterator>
  hashmap(_InputIterator __f, _InputIterator __l, size_type __n,
           const hasher& __hf)
    : _M_ht(__n, __hf, key_equal())
    { _M_ht.insert_unique(__f, __l); }
  
  template <class _InputIterator>
  hashmap(_InputIterator __f, _InputIterator __l, size_type __n,
           const hasher& __hf, const key_equal& __eql)
    : _M_ht(__n, __hf, __eql)
    { _M_ht.insert_unique(__f, __l); }

  ~hashmap() {}

public:
  size_type size()     const { return _M_ht.size(); }
  size_type max_size() const { return _M_ht.max_size(); }
  
  bool empty()         const { return _M_ht.empty(); }
  
  void swap(hashmap& __hs) { _M_ht.swap(__hs._M_ht); }

  template <class _K1, class _T1, class _HF, class _EqK, class _Al>
  friend bool operator== (const hashmap<_K1, _T1, _HF, _EqK>&,
                          const hashmap<_K1, _T1, _HF, _EqK>&);

  iterator begin() { return _M_ht.begin(); }
  iterator end()   { return _M_ht.end(); }
  
  const_iterator begin() const { return _M_ht.begin(); }
  const_iterator end()   const { return _M_ht.end(); }

public:
  std::pair<iterator,bool> insert(const value_type& __obj) { return _M_ht.insert_unique(__obj); }
  
  template <class _InputIterator>
  void insert(_InputIterator __f, _InputIterator __l) { _M_ht.insert_unique(__f,__l); }
  
  std::pair<iterator,bool> Insert_Noresize(const value_type& __obj) 
  { return _M_ht.insert_unique_noresize(__obj); }    

  iterator       find(const key_type& __key)       { return _M_ht.find(__key); }
  const_iterator find(const key_type& __key) const { return _M_ht.find(__key); }

  _Val& operator[](const key_type& __key) {
    return _M_ht.find_or_insert(value_type(__key, _Val())).second;
  }

  size_type count(const key_type& __key) const { return _M_ht.count(__key); }
  
  std::pair<iterator, iterator>             equal_range(const key_type& __key)       { return _M_ht.equal_range(__key); }
  std::pair<const_iterator, const_iterator> equal_range(const key_type& __key) const { return _M_ht.equal_range(__key); }

  size_type erase(const key_type& __key) { return _M_ht.erase(__key); }

  void erase(iterator __it)              { _M_ht.erase(__it); }
  void erase(iterator __f, iterator __l) { _M_ht.erase(__f, __l); }
  void clear()                           { _M_ht.clear(); }

  void resize(size_type __hint) { _M_ht.resize(__hint); }
  
  size_type bucket_count()                 const { return _M_ht.bucket_count(); }
  size_type max_bucket_count()             const { return _M_ht.max_bucket_count(); }
  size_type elems_in_bucket(size_type __n) const { return _M_ht.elems_in_bucket(__n); }
};

template <class _Key, class _Val, class _HashFcn, class _EqlKey>
inline bool 
operator==(const hashmap<_Key,_Val,_HashFcn,_EqlKey>& __hm1,
           const hashmap<_Key,_Val,_HashFcn,_EqlKey>& __hm2)
{
  return __hm1._M_ht == __hm2._M_ht;
}

template <class _Key, class _Val, class _HashFcn, class _EqlKey>
inline bool 
operator!=(const hashmap<_Key,_Val,_HashFcn,_EqlKey>& __hm1,
           const hashmap<_Key,_Val,_HashFcn,_EqlKey>& __hm2) {
  return !(__hm1 == __hm2);
}

template <class _Key, class _Val, class _HashFcn, class _EqlKey>
inline void 
swap(hashmap<_Key,_Val,_HashFcn,_EqlKey>& __hm1,
     hashmap<_Key,_Val,_HashFcn,_EqlKey>& __hm2)
{
  __hm1.swap(__hm2);
}

} //namespace daim

//----------------------------------------------------------------------
#endif // dmHashMap_h 


