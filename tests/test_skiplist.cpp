#include "cpputils/skiplist.h"
#include "cpputils/assert.h"
#include <iostream>
#include <set>
#include <sys/time.h>
#include <random>
using namespace std;
using namespace cpputils;

static void TestConst(const SkipListSet<int>& sl) {
    auto it = sl.Lookup(50);
    ASSERT_TRUE(it != sl.GetEndIterator());
    ASSERT_EQ(50, *it);
}

static void TestSkipListSet(void) {
    cout << "----- test " << __func__ << " begin -----" << endl;

    SkipListSet<int> sl;
    for (int i = 100; i > 0; i -= 10) {
        auto ret_pair = sl.Insert(i);
        ASSERT_TRUE(ret_pair.second);
    }
    for (int i = 100; i > 0; i -= 10) {
        auto ret_pair = sl.Insert(i);
        ASSERT_TRUE(!ret_pair.second);
    }

    auto it = sl.GetBeginIterator();
    for (int i = 10; i <= 100; i += 10) {
        ASSERT_EQ(i, *it);
        ++it;
    }

    TestConst(sl);

    it = sl.Lookup(50);
    ASSERT_TRUE(it != sl.GetEndIterator());
    ASSERT_EQ(50, *it);

    it = sl.Lookup(101);
    ASSERT_TRUE(it == sl.GetEndIterator());

    sl.Remove(20);

    it = sl.LookupGreaterOrEqual(21);
    ASSERT_EQ(30, *it);

    it = sl.LookupLessThan(21);
    ASSERT_EQ(10, *it);

    it = sl.LookupLessThan(1000);
    ASSERT_EQ(100, *it);

    it = sl.LookupLessThan(1);
    ASSERT_TRUE(it == sl.GetEndIterator());

    int ret = 0;
    sl.RemoveGreaterOrEqual(21, &ret);
    ASSERT_EQ(30, ret);

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

static void PrepareTestData(vector<uint32_t>* data) {
    std::mt19937 gen(time(nullptr));
    for (uint32_t i = 0; i < 555555; ++i) {
        data->push_back(gen());
    }
}

uint64_t diff_time_usec(struct timeval end, const struct timeval* begin) {
    if (end.tv_usec < begin->tv_usec) {
        --end.tv_sec;
        end.tv_usec += 1000000;
    }
    return (end.tv_sec - begin->tv_sec) * 1000000 + (end.tv_usec - begin->tv_usec);
}

static void TestPerf() {
    cout << "----- performance test begin -----" << endl;
    vector<uint32_t> test_data;
    PrepareTestData(&test_data);

    set<uint32_t> st;
    struct timeval st_begin, st_end;
    gettimeofday(&st_begin, nullptr);
    for (auto it : test_data) {
        st.insert(it);
    }
    gettimeofday(&st_end, nullptr);

    SkipListSet<uint32_t> sl;
    struct timeval sl_begin, sl_end;
    gettimeofday(&sl_begin, nullptr);
    for (auto it : test_data) {
        sl.Insert(it);
    }
    gettimeofday(&sl_end, nullptr);

    cout << "skiplist insert cost " << diff_time_usec(sl_end, &sl_begin) / 1000.0 << " ms, "
         << "std::set insert cost " << diff_time_usec(st_end, &st_begin) / 1000.0 << " ms."
         << endl;
}

int main(void) {
    TestSkipListSet();
    TestSkipListMap();
    TestPerf();
    return 0;
}
