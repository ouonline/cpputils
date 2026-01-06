#ifndef __CPPUTILS_STRING_UTILS_H__
#define __CPPUTILS_STRING_UTILS_H__

#include <cstring>
#include <string>
#include <vector>
#include <functional>

namespace cpputils {

std::string StringReplace(const char* text, unsigned int tlen,
                          const char* search, unsigned int slen,
                          const char* replace, unsigned int rlen);

bool StringEndsWith(const char* text, unsigned int tlen, const char* suffix,
                    unsigned int slen);

// reutrns the number of chars removed at the end of `text`
unsigned int StringTrim(const char* text, unsigned int tlen, char c);

class StringSplitter final {
public:
    StringSplitter() : m_cursor(nullptr), m_end(nullptr) {}
    StringSplitter(const char* s, unsigned int l) : m_cursor(s), m_end(s + l) {}
    void Reset(const char* s, unsigned int l) {
        m_cursor = s;
        m_end = s + l;
    }
    /* string ends if pair::first is null */
    std::pair<const char*, unsigned int> Next(const char* delim,
                                              unsigned int dlen);

private:
    const char* m_cursor;
    const char* m_end;
};

}

#endif
