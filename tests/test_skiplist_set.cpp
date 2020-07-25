#include "cpputils/skiplist.h"
#include "cpputils/assert.h"
#include <iostream>
using namespace std;
using namespace outils;

int main(void) {
    srand(time(nullptr));

    SkipListSet<int> sl;
    for (int i = 100; i > 0; i -= 10) {
        auto ret_pair = sl.Insert(i);
        ASSERT_TRUE(ret_pair.second);
    }
    for (int i = 100; i > 0; i -= 10) {
        auto ret_pair = sl.Insert(i, true);
        ASSERT_TRUE(ret_pair.second);
    }
    for (int i = 100; i > 0; i -= 10) {
        auto ret_pair = sl.Insert(i);
        ASSERT_TRUE(!ret_pair.second);
    }

    auto value = sl.Lookup(50);
    ASSERT_TRUE(value != nullptr);
    ASSERT_EQ(*value, 50);

    value = sl.Lookup(101);
    ASSERT_TRUE(value == nullptr);

    sl.Remove(20);

    value = sl.LookupGreaterOrEqual(21);
    ASSERT_EQ(*value, 30);

    int ret = 0;
    sl.RemoveGreaterOrEqual(21, &ret);
    ASSERT_EQ(ret, 30);

    return 0;
}
