#ifndef __TEXT_UTILS_H__
#define __TEXT_UTILS_H__

#include <cstring>
#include <string>
#include <functional>

namespace outils {

void TextSplit(const char* str, unsigned int len,
               const char* delim, unsigned int delim_len,
               const std::function<bool (const char* s, unsigned int l)>& f);

std::string TextReplace(const char* text, unsigned int tlen,
                        const char* search, unsigned int slen,
                        const char* replace, unsigned int rlen);

bool TextEndsWith(const char* text, unsigned int tlen,
                  const char* suffix, unsigned int slen);

// reutrn the number of chars removed at the end of `text`
unsigned int TextTrim(const char* text, unsigned int tlen, char c);

}

#endif
