#include "stdafx.h"
#include "StringUtils.h"
#include <ctype.h>
#include <stdint.h>
#include <wctype.h>
#include <algorithm>

uint32_t strhash(const char *str, uint32_t seed)
{
  if(str == NULL)
    return seed;

  uint32_t hash = seed;
  int c = *str;
  str++;

  while(c)
  {
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    c = *str;
    str++;
  }

  return hash;
}

string strlower(const string &str)
{
  string newstr(str);
  transform(newstr.begin(), newstr.end(), newstr.begin(), tolower);
  return newstr;
}

wstring strlower(const wstring &str)
{
  wstring newstr(str);
  transform(newstr.begin(), newstr.end(), newstr.begin(), towlower);
  return newstr;
}

string strupper(const string &str)
{
  string newstr(str);
  transform(newstr.begin(), newstr.end(), newstr.begin(), toupper);
  return newstr;
}

wstring strupper(const wstring &str)
{
  wstring newstr(str);
  transform(newstr.begin(), newstr.end(), newstr.begin(), towupper);
  return newstr;
}

std::string trim(const std::string &str)
{
  size_t start = str.find_first_not_of("\t \n");
  size_t end = str.find_last_not_of("\t \n");

  // no non-whitespace characters, return the empty string
  if(start == std::string::npos)
    return "";

  // searching from the start found something, so searching from the end must have too.
  return str.substr(start, end - start + 1);
}
