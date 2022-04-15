#include "cpputils/string_utils.h"
using namespace outils;

#include <iostream>
using namespace std;

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
    cout << "-----------------------------" << endl;
    TestStringReplace();

    return 0;
}
