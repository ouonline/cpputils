#include "cpputils/skiplist.h"
#include "cpputils/assert.h"
#include <iostream>
#include <set>
using namespace std;
using namespace outils;

static void TestConst(const SkipListSet<int>& sl) {
    auto it = sl.Lookup(50);
    ASSERT_TRUE(it != sl.GetEndIterator());
    ASSERT_EQ(*it, 50);
}

static void TestSkipListSet(void) {
    cout << "----- test " << __func__ << " begin -----" << endl;

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

    TestConst(sl);

    auto it = sl.Lookup(50);
    ASSERT_TRUE(it != sl.GetEndIterator());
    ASSERT_EQ(*it, 50);

    it = sl.Lookup(101);
    ASSERT_TRUE(it == sl.GetEndIterator());

    sl.Remove(20);

    it = sl.LookupGreaterOrEqual(21);
    ASSERT_EQ(*it, 30);

    it = sl.LookupPrev(21);
    ASSERT_EQ(*it, 10);

    int ret = 0;
    sl.RemoveGreaterOrEqual(21, &ret);
    ASSERT_EQ(ret, 30);

    for (auto it = sl.GetBeginIterator(); it != sl.GetEndIterator(); ++it) {
        cout << "    " << *it << endl;
    }
    cout << "----- test " << __func__ << " end -----" << endl;
}

static void TestSkipListMap(void) {
    cout << "----- test " << __func__ << " begin -----" << endl;

    struct TestValue {
        TestValue(int _a = 5) : a(_a) {
            cout << "construct a = " << a << endl;
        }
        TestValue(const TestValue& t) {
            a = t.a;
            cout << "copy construct a = " << t.a << endl;
        }
        ~TestValue() { cout << "destroy TestValue[" << a << "]" << endl; }
        int a;
    };

    SkipListMap<int, TestValue> sl;
    for (int i = 0; i < 100; i += 10) {
        auto ret_pair = sl.Insert(std::pair<int, TestValue>(i, TestValue(i)));
        ASSERT_TRUE(ret_pair.second);
    }

    sl.Remove(20);

    sl.Clear();
    ASSERT_TRUE(sl.IsEmpty());

    for (int i = 0; i < 100; i += 10) {
        auto ret_pair = sl.Insert(std::pair<int, TestValue>(i, TestValue(i)));
        ASSERT_TRUE(ret_pair.second);
    }

    cout << "----- test " << __func__ << " end -----" << endl;
}

int main(void) {
    TestSkipListSet();
    TestSkipListMap();
    return 0;
}
