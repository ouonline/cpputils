#include "cpputils/skiplist.h"
#include "cpputils/assert.h"
#include <iostream>
#include <set>
using namespace std;
using namespace outils;

static void TestConst(const SkipListSet<int>& sl) {
    int* value = sl.Lookup(50);
    ASSERT_TRUE(value != nullptr);
    ASSERT_EQ(*value, 50);
}

static void TestSkipListSet(void) {
    cout << "----------- TestSkipListSet ----------" << endl;

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

    cout << "    ----- test func end -----" << endl;
}

static void TestSkipListMap(void) {
    cout << "--------- TestSkipListMap ---------" << endl;

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

    cout << "    ----- test func end -----" << endl;
}

static void TestSkipListMap2(void) {
    cout << "--------- TestSkipListMap2 ---------" << endl;

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

    cout << "    ----- begin -----" << endl;
    sl.ForEach([] (const pair<int, TestValue>& p) -> bool {
        cout << "    " << p.first << " : " << p.second.a << endl;
        return true;
    });
    cout << "    ----- end -----" << endl;

    sl.Clear();
    ASSERT_TRUE(sl.IsEmpty());

    for (int i = 0; i < 100; i += 10) {
        auto ret_pair = sl.Insert(std::pair<int, TestValue>(i, TestValue(i)));
        ASSERT_TRUE(ret_pair.second);
    }

    cout << "    ----- test func end -----" << endl;
}

int main(void) {
    TestSkipListSet();
    TestSkipListMap();
    TestSkipListMap2();
    return 0;
}
