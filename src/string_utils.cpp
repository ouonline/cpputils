#include "cpputils/string_utils.h"
using namespace std;

namespace cpputils {

static const char* MemMem(const char* haystack, unsigned int haystack_len,
                          const char* needle, unsigned int needle_len) {
    if (!haystack || haystack_len == 0 || !needle || needle_len == 0) {
        return nullptr;
    }

    for (auto h = haystack; haystack_len >= needle_len; ++h, --haystack_len) {
        if (memcmp(h, needle, needle_len) == 0) {
            return h;
        }
    }

    return nullptr;
}

string StringReplace(const char* text, unsigned int tlen, const char* search,
                     unsigned int slen, const char* replace,
                     unsigned int rlen) {
    string ret;
    const char* end = text + tlen;

    while (text < end) {
        auto cursor = MemMem(text, tlen, search, slen);
        if (!cursor) {
            return ret + string(text, tlen);
        }

        if (cursor > text) {
            ret.append(text, cursor - text);
        }

        ret.append(replace, rlen);

        cursor += slen;
        text = cursor;
        tlen = end - cursor;
    }

    return ret;
}

bool StringEndsWith(const char* text, unsigned int tlen, const char* suffix,
                    unsigned int slen) {
    if (tlen < slen) {
        return false;
    }

    return (memcmp(text + tlen - slen, suffix, slen) == 0);
}

unsigned int StringTrim(const char* text, unsigned int tlen, char c) {
    unsigned int pos = tlen;
    while (pos > 0) {
        --pos;
        if (text[pos] != c) {
            ++pos;
            break;
        }
    }

    return (tlen - pos);
}

pair<const char*, unsigned int> StringSplitter::Next(const char* delim,
                                                     unsigned int delim_len) {
    if (m_next_offset > m_len) {
        return make_pair(nullptr, 0);
    }

    auto text = m_str + m_next_offset;

    // the last empty field
    if (m_next_offset == m_len) {
        ++m_next_offset;
        return make_pair(text, 0);
    }

    auto text_len = m_len - m_next_offset;
    auto cursor = MemMem(text, text_len, delim, delim_len);
    if (cursor) {
        m_next_offset = cursor + delim_len - m_str;
        return make_pair(text, cursor - text);
    }

    m_next_offset = m_len;
    return make_pair(text, text_len);
}

}
