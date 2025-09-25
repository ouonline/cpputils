#include "cpputils/skiplist.h"
#include <iostream>
#include <set>
#include <sys/time.h>
#include <random>
using namespace std;
using namespace cpputils;

#undef NDEBUG
#include <assert.h>

static void TestConst(const SkipListSet<int>& sl) {
    auto it = sl.Lookup(50);
    assert(it != sl.GetEndIterator());
    assert(*it == 50);
}

static void TestSkipListSet(void) {
    SkipListSet<int> sl;
    constexpr int expected_values[] = {100, 90, 80, 70, 60, 50, 40, 30, 20, 10};
    constexpr int nr_values = 10;

    cout << "----- test insert -----" << endl;

    for (int i = 0; i < nr_values; ++i) {
        auto ret_pair = sl.Insert(expected_values[i]);
        assert(ret_pair.second);
    }
    for (auto it = sl.GetBeginIterator(); it != sl.GetEndIterator(); ++it) {
        cout << "    " << *it << endl;
    }

    for (int i = nr_values - 1; i >= 0; --i) {
        auto ret_pair = sl.Insert(expected_values[i]);
        assert(!ret_pair.second);
    }

    cout << "----- test iterator -----" << endl;

    auto it = sl.GetBeginIterator();
    for (int i = nr_values - 1; i >= 0; --i) {
        assert(*it == expected_values[i]);
        ++it;
    }
    assert(it == sl.GetEndIterator());

    cout << "----- test lookup -----" << endl;

    TestConst(sl);

    it = sl.Lookup(50);
    assert(it != sl.GetEndIterator());
    assert(*it == 50);

    it = sl.Lookup(101);
    assert(it == sl.GetEndIterator());

    cout << "----- test lookup greater or equal -----" << endl;

    it = sl.LookupGreaterEqual(21);
    assert(*it == 30);

    it = sl.LookupGreaterEqual(50);
    assert(*it == 50);

    it = sl.LookupGreaterEqual(51);
    assert(*it == 60);

    cout << "----- test lookup less than -----" << endl;

    it = sl.LookupLessThan(21);
    assert(*it == 20);

    it = sl.LookupLessThan(20);
    assert(*it == 10);

    it = sl.LookupLessThan(1000);
    assert(*it == 100);

    it = sl.LookupLessThan(1);
    assert(it == sl.GetEndIterator());

    cout << "----- test remove -----" << endl;

    int ret = 0;
    sl.Remove(20, &ret);
    assert(ret == 20);

    sl.RemoveGreaterEqual(21, &ret);
    assert(ret == 30);

    sl.RemoveGreaterEqual(40, &ret);
    assert(ret == 40);

    sl.RemoveGreaterEqual(99, &ret);
    assert(ret == 100);

    for (auto it = sl.GetBeginIterator(); it != sl.GetEndIterator(); ++it) {
        cout << "    " << *it << endl;
    }
}

static void TestSkipListMap(void) {
    cout << "----- test user defined value -----" << endl;

    struct TestValue {
        TestValue(int _a = 5) : a(_a) {}
        TestValue(const TestValue& t) {
            a = t.a;
        }
        int a;
    };

    SkipListMap<int, TestValue> sl;
    for (int i = 0; i < 100; i += 10) {
        auto ret_pair = sl.Insert(std::pair<int, TestValue>(i, TestValue(i)));
        assert(ret_pair.second);
    }

    sl.Remove(20);

    sl.Clear();
    assert(sl.IsEmpty());

    for (int i = 0; i < 100; i += 10) {
        auto ret_pair = sl.Insert(std::pair<int, TestValue>(i, TestValue(i)));
        assert(ret_pair.second);
    }
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
    return (end.tv_sec - begin->tv_sec) * 1000000 +
        (end.tv_usec - begin->tv_usec);
}

static void TestPerf() {
    cout << "----- test insert perf -----" << endl;
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

    cout << "skiplist insert cost "
         << diff_time_usec(sl_end, &sl_begin) / 1000.0 << " ms, "
         << "std::set insert cost "
         << diff_time_usec(st_end, &st_begin) / 1000.0 << " ms." << endl;

    cout << "----- test lookup perf -----" << endl;

    gettimeofday(&st_begin, nullptr);
    for (auto it : test_data) {
        st.find(it);
    }
    gettimeofday(&st_end, nullptr);

    gettimeofday(&sl_begin, nullptr);
    for (auto it : test_data) {
        sl.Lookup(it);
    }
    gettimeofday(&sl_end, nullptr);

    cout << "skiplist lookup cost "
         << diff_time_usec(sl_end, &sl_begin) / 1000.0 << " ms, "
         << "std::set lookup cost "
         << diff_time_usec(st_end, &st_begin) / 1000.0 << " ms." << endl;

    cout << "----- test traversal perf -----" << endl;

    gettimeofday(&st_begin, nullptr);
    for (auto it = st.begin(); it != st.end(); ++it)
        ;
    gettimeofday(&st_end, nullptr);

    gettimeofday(&sl_begin, nullptr);
    for (auto it = sl.GetBeginIterator(); it != sl.GetEndIterator(); ++it)
        ;
    gettimeofday(&sl_end, nullptr);

    cout << "skiplist traversal cost "
         << diff_time_usec(sl_end, &sl_begin) / 1000.0 << " ms, "
         << "std::set traversal cost "
         << diff_time_usec(st_end, &st_begin) / 1000.0 << " ms." << endl;
}

int main(void) {
    TestSkipListSet();
    TestSkipListMap();
    TestPerf();
    return 0;
}
