#ifndef dmHashString_h
#define dmHashString_h

#include "dmHashfuncs.h"

namespace daim {

template<> struct hash<dmACString>
{
  size_t operator()(const dmACString& __s) const { 
     return __s.IsEmpty()?0:__dflt_hash_string_charT(__s.get()); 
  }
};

template<> struct hash<dmAString>
{
  size_t operator()(const dmAString& __s) const { 
     return __s.IsEmpty()?0:__dflt_hash_string_charT(__s.get()); 
  }
};

}; // namespace daim

#endif
