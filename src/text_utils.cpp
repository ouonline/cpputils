#include "cpputils/text_utils.h"
using namespace std;

namespace outils {

void TextSplit(const char* str, unsigned int len,
               const char* delim, unsigned int delim_len,
               const function<bool (const char* s, unsigned int l)>& f) {
    const char* end = str + len;

    while (str < end) {
        const char* cursor = (const char*)memmem(str, len, delim, delim_len);
        if (!cursor) {
            f(str, end - str);
            return;
        }

        if (!f(str, cursor - str)) {
            return;
        }

        cursor += delim_len;
        str = cursor;
        len = end - cursor;
    }

    f("", 0); // the last empty field
}

string TextReplace(const char* text, unsigned int tlen,
                   const char* search, unsigned int slen,
                   const char* replace, unsigned int rlen) {
    string ret;
    const char* end = text + tlen;

    while (text < end) {
        const char* cursor = (const char*)memmem(text, tlen, search, slen);
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

bool TextEndsWith(const char* text, unsigned int tlen,
                  const char* suffix, unsigned int slen) {
    if (tlen < slen) {
        return false;
    }

    return (memcmp(text + tlen - slen, suffix, slen) == 0);
}

unsigned int TextTrim(const char* text, unsigned int tlen, char c) {
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

}
