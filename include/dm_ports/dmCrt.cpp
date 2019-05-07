
// This is a GNU extension not available on some platforms
// See wcsncasecmp(3) and wcscasecmp(3) on linux man page

/* Compare S1 and S2, ignoring case, returning less than, equal to or
   greater than zero if S1 is lexicographically less than,
   equal to or greater than S2.  */
inline int wcscasecmp (const wchar_t *s1, const wchar_t *s2)
{
  wint_t c1, c2;

  if (s1 == s2)
    return 0;

  do {
      c1 = towlower (*s1++);
      c2 = towlower (*s2++);
      if (c1 == L'\0')
        break;
  } while (c1 == c2);

  return c1 - c2;
}


/* Compare no more than N wide characters of S1 and S2,
   ignoring case, returning less than, equal to or
   greater than zero if S1 is lexicographically less
   than, equal to or greater than S2.  */

inline int wcsncasecmp (const wchar_t *s1, const wchar_t *s2, size_t n)
{
  wint_t c1, c2;

  if (s1 == s2 || n == 0)
    return 0;

  do {
    c1 = towlower (*s1++);
    c2 = towlower (*s2++);
    if (c1 == L'\0' || c1 != c2)
        return c1 - c2;
  } while (--n > 0);

  return c1 - c2;
}
