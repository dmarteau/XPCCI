#ifndef dmHashfuncs_h
#define dmHashfuncs_h

namespace daim {

template <class _Key> struct hash { };

template<class char_type> 
inline size_t __dflt_hash_string_charT(const char_type* __s)
{
  // Use the djb2 algorithm
  unsigned long __h = 5381;
  for ( ; *__s; ++__s)
    __h = ((__h << 5) + __h) + *__s;
  
  return size_t(__h);
}

template<> struct hash<char*>
{
  size_t operator()(const char* __s) const { return __dflt_hash_string_charT(__s); }
};

template<> struct hash<char> {
  size_t operator()(char __x) const { return __x; }
};
template<> struct hash<unsigned char> {
  size_t operator()(unsigned char __x) const { return __x; }
};
template<> struct hash<signed char> {
  size_t operator()(unsigned char __x) const { return __x; }
};
template<> struct hash<short> {
  size_t operator()(short __x) const { return __x; }
};
template<> struct hash<unsigned short> {
  size_t operator()(unsigned short __x) const { return __x; }
};
template<> struct hash<int> {
  size_t operator()(int __x) const { return __x; }
};
template<> struct hash<unsigned int> {
  size_t operator()(unsigned int __x) const { return __x; }
};
template<> struct hash<long> {
  size_t operator()(long __x) const { return __x; }
};
template<> struct hash<unsigned long> {
  size_t operator()(unsigned long __x) const { return __x; }
};
template<> struct hash<void*> {
  size_t operator()(void* __x) const { return (size_t)__x; }
};


}; // namespace daim

#endif // dmHashfuncs_h 
