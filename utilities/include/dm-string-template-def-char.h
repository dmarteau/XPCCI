
#define CharT char
#define CharT_is_char 1
#define other_CharT wchar_t
#define to_CharT( s ) s

#define dmTAString_CharT dmACString
#define dmTString_Type dmCString
#define dmTString_Incompatible_Type  dmAString

#define dmTString_implementation __dm_string_implementation<CharT>
