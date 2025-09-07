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

bool StringEndsWith(const char* text, unsigned int tlen,
                    const char* suffix, unsigned int slen);

// reutrns the number of chars removed at the end of `text`
unsigned int StringTrim(const char* text, unsigned int tlen, char c);

class StringSplitter final {
public:
    StringSplitter(const char* s, unsigned int l) : s_(s), l_(l), next_offset_(0) {}
    /* string ends if pair::first is null */
    std::pair<const char*, unsigned int> Next(const char* delim, unsigned int delim_len);

private:
    const char* s_;
    unsigned int l_;
    unsigned int next_offset_;
};

inline std::vector<std::string> StringSplit(const std::string& text,
                                            const std::string& delim,
                                            bool keep_empty = false) {
    std::vector<std::string> res;

    StringSplitter splitter(text.data(), text.size());
    while (true) {
        auto ret_pair = splitter.Next(delim.data(), delim.size());
        if (!ret_pair.first) {
            break;
        }
        if (!keep_empty && ret_pair.second == 0) {
            continue;
        }
        res.emplace_back(ret_pair.first, ret_pair.second);
    }
    return res;
}

}

#endif
