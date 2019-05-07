
#define CharT wchar_t
#define CharT_is_unichar 1
#define other_CharT char
#define to_CharT( s ) DM_LL(s)

#define dmTAString_CharT dmAString
#define dmTString_Type dmString
#define dmTString_Incompatible_Type dmACString

#define dmTString_implementation __dm_string_implementation<CharT>
