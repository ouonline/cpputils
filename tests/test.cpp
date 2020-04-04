#include "text-utils/text_utils.h"
using namespace utils;

#include <iostream>
using namespace std;

static void TestTextSplit() {
    const char* text = "abc,,de,f,g,,";
    unsigned int counter = 1;
    auto f = [&counter] (const char* s, unsigned int l) -> bool {
        cout << counter << " -> |" << string(s, l) << "|" << endl;
        ++counter;
        return true;
    };

    TextSplit(text, strlen(text), ",", 1, f);
}

static void TestTextReplace() {
    const string text = "1ecb2ecb3ecbecbecb4444ecb5ecb";
    const string search = "ecb";
    const string replace = "x";
    cout << "after replace 1 -> " << TextReplace(text.data(), text.size(),
                                                 search.data(), search.size(),
                                                 replace.data(), replace.size())
         << endl
         << "after replace 3 -> " << TextReplace(text.data(), text.size(),
                                                 search.data(), search.size(),
                                                 replace.data(), replace.size())
         << endl
         << "after replace all -> " << TextReplace(text.data(), text.size(),
                                                   search.data(), search.size(),
                                                   replace.data(), replace.size())
         << endl;
}

int main(void) {
    TestTextSplit();
    cout << "-----------------------------" << endl;
    TestTextReplace();

    return 0;
}
