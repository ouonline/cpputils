#include "cpputils/skiplist.h"
#include "cpputils/assert.h"
#include <iostream>
#include <set>
using namespace std;
using namespace outils;

static void test_skiplist_set(void) {
    cout << "----------- test_skiplist_set ----------" << endl;

    srand(time(nullptr));

    SkipListSet<int> sl;
    for (int i = 100; i > 0; i -= 10) {
        auto ret_pair = sl.Insert(i);
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
}

static void test_skiplist_map(void) {
    cout << "--------- test_skiplist_map ---------" << endl;

    SkipListMap<int, set<int>> sl;
    for (int i = 0; i < 100; i += 10) {
        auto ret_pair = sl.Insert(std::make_pair(i, set<int>()));
        ret_pair.first->second.insert(i + 5);
        ret_pair.first->second.insert(i + 10);
        ASSERT_TRUE(ret_pair.second);
    }

    cout << "    ----- begin -----" << endl;
    sl.ForEach([] (const pair<int, set<int>>& p) -> bool {
        cout << "    " << p.first << " :";
        for (auto x : p.second) {
            cout << " " << x;
        }
        cout << endl;
        return true;
    });
    cout << "    ----- end -----" << endl;
}

static void test_skiplist_map2(void) {
    cout << "--------- test_skiplist_map2 ---------" << endl;

    struct TestValue {
        TestValue(int _a = 5) : a(_a) {}
        ~TestValue() { cout << "destroy TestValue[" << a << "]" << endl; }
        int a;
    };

    SkipListMap<int, TestValue> sl;
    for (int i = 0; i < 100; i += 10) {
        auto ret_pair = sl.Insert(std::pair<int, TestValue>(i, TestValue(i)));
        ASSERT_TRUE(ret_pair.second);
    }

    cout << "    ----- begin -----" << endl;
    sl.ForEach([] (const pair<int, TestValue>& p) -> bool {
        cout << "    " << p.first << " : " << p.second.a << endl;
        return true;
    });
    cout << "    ----- end -----" << endl;
}

int main(void) {
    test_skiplist_set();
    test_skiplist_map();
    test_skiplist_map2();
    return 0;
}
