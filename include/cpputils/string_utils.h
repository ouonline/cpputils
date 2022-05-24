#ifndef __CPPUTILS_STRING_UTILS_H__
#define __CPPUTILS_STRING_UTILS_H__

#include <cstring>
#include <string>
#include <functional>

namespace cpputils {

void StringSplit(const char* str, unsigned int len,
               const char* delim, unsigned int delim_len,
               const std::function<bool (const char* s, unsigned int l)>& f);

std::string StringReplace(const char* text, unsigned int tlen,
                        const char* search, unsigned int slen,
                        const char* replace, unsigned int rlen);

bool StringEndsWith(const char* text, unsigned int tlen,
                  const char* suffix, unsigned int slen);

// reutrns the number of chars removed at the end of `text`
unsigned int StringTrim(const char* text, unsigned int tlen, char c);

}

#endif
