#include "cpputils/string_utils.h"
using namespace std;

namespace cpputils {

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

static const char* MemMem(const char* text, unsigned int tlen,
                          const char* pattern, unsigned int plen) {
    for (auto s = text; tlen >= plen; ++s, --tlen) {
        if (memcmp(s, pattern, plen) == 0) {
            return s;
        }
    }
    return nullptr;
}

string StringReplace(const char* text, unsigned int tlen, const char* search,
                     unsigned int slen, const char* replace,
                     unsigned int rlen) {
    if (!text || tlen == 0) {
        return string();
    }
    if (!search || slen == 0 || !replace || rlen == 0) {
        return string(text, tlen);
    }

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

pair<const char*, unsigned int> StringSplitter::Next(const char* delim,
                                                     unsigned int dlen) {
    if (m_cursor > m_end) {
        return make_pair(nullptr, 0);
    }

    // the last empty field
    if (m_cursor == m_end) {
        ++m_cursor;
        return make_pair(m_end, 0);
    }

    const char* begin = m_cursor;

    const char* end;
    if (dlen == 1) {
        end = (const char*)memchr(m_cursor, *delim, m_end - m_cursor);
    } else {
        end = MemMem(m_cursor, m_end - m_cursor, delim, dlen);
    }

    if (end) {
        m_cursor = end + dlen;
        return make_pair(begin, end - begin);
    }

    m_cursor = m_end + 1;
    return make_pair(begin, m_end - begin);
}

}
