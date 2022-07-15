#include "cpputils/string_utils.h"
using namespace cpputils;

#include <iostream>
using namespace std;

#undef NDEBUG
#include <assert.h>

static void TestStringSplit() {
    const char* text = "abc,,de,f,g,,";
    unsigned int counter = 1;
    auto f = [&counter] (const char* s, unsigned int l) -> bool {
        cout << counter << " -> |" << string(s, l) << "|" << endl;
        ++counter;
        return true;
    };

    StringSplit(text, strlen(text), ",", 1, f);
}

static void TestStringSplitter() {
    const char* text = "abc,,de,f,g,,";
    StringSplitter splitter(text, strlen(text));

    auto ret_pair = splitter.Next(",", 1);
    assert(ret_pair.first);
    assert(string(ret_pair.first, ret_pair.second) == string("abc"));

    ret_pair = splitter.Next(",", 1);
    assert(ret_pair.first);
    assert(string(ret_pair.first, ret_pair.second) == string());

    ret_pair = splitter.Next(",", 1);
    assert(ret_pair.first);
    assert(string(ret_pair.first, ret_pair.second) == string("de"));

    ret_pair = splitter.Next(",", 1);
    assert(ret_pair.first);
    assert(string(ret_pair.first, ret_pair.second) == string("f"));

    ret_pair = splitter.Next(",", 1);
    assert(ret_pair.first);
    assert(string(ret_pair.first, ret_pair.second) == string("g"));

    ret_pair = splitter.Next(",", 1);
    assert(ret_pair.first);
    assert(string(ret_pair.first, ret_pair.second) == string());

    ret_pair = splitter.Next(",", 1);
    assert(ret_pair.first);
    assert(string(ret_pair.first, ret_pair.second) == string());

    ret_pair = splitter.Next(",", 1);
    assert(!ret_pair.first);
}

static void TestStringReplace() {
    const string text = "1ecb2ecb3ecbecbecb4444ecb5ecb";
    const string search = "ecb";
    const string replace = "x";
    cout << "after replace 1 -> " << StringReplace(text.data(), text.size(),
                                                   search.data(), search.size(),
                                                   replace.data(), replace.size())
         << endl
         << "after replace 3 -> " << StringReplace(text.data(), text.size(),
                                                   search.data(), search.size(),
                                                   replace.data(), replace.size())
         << endl
         << "after replace all -> " << StringReplace(text.data(), text.size(),
                                                     search.data(), search.size(),
                                                     replace.data(), replace.size())
         << endl;
}

int main(void) {
    TestStringSplit();
    TestStringReplace();
    TestStringSplitter();

    return 0;
}
