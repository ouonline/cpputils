#include "cpputils/string_utils.h"
using namespace cpputils;

#include <iostream>
using namespace std;

#undef NDEBUG
#include <assert.h>

static vector<string> StringSplit(const string& text, const string& delim,
                                  bool skip_empty) {
    vector<string> res;
    StringSplitter splitter(text.data(), text.size());
    while (true) {
        auto ret_pair = splitter.Next(",", 1);
        if (!ret_pair.first) {
            break;
        }
        if (skip_empty && ret_pair.second == 0) {
            continue;
        }
        res.emplace_back(ret_pair.first, ret_pair.second);
    }
    return res;
}

static void TestStringSplitter() {
    string text = "abc,,de,f,g,,";
    vector<string> expected = {"abc", "", "de", "f", "g", "", ""};
    vector<string> ret = StringSplit(text, ",", false);
    assert(ret == expected);

    text = "abc,,de,f,g";
    expected = {"abc", "", "de", "f", "g"};
    ret = StringSplit(text, ",", false);
    assert(ret == expected);

    text = "abc,,de,f,g,,";
    expected = {"abc", "de", "f", "g"};
    ret = StringSplit(text, ",", true);
    assert(ret == expected);
}

static void TestStringReplace() {
    const string text = "1ecb2ecb3ecbecbecb4444ecb5ecb";
    const string search = "ecb";
    const string replace = "x";
    cout << "after replace 1 -> "
         << StringReplace(text.data(), text.size(), search.data(),
                          search.size(), replace.data(), replace.size())
         << endl
         << "after replace 3 -> "
         << StringReplace(text.data(), text.size(), search.data(),
                          search.size(), replace.data(), replace.size())
         << endl
         << "after replace all -> "
         << StringReplace(text.data(), text.size(), search.data(),
                          search.size(), replace.data(), replace.size())
         << endl;
}

int main(void) {
    TestStringReplace();
    TestStringSplitter();

    return 0;
}
