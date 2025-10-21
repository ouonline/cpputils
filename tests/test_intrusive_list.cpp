#include "cpputils/intrusive_list.h"
using namespace std;
using namespace cpputils;

#undef NDEBUG
#include <assert.h>

struct Tag1 {};
struct Tag2 {};

struct MyStruct final
    : public IntrusiveListNode<MyStruct, Tag1>
    , public IntrusiveListNode<MyStruct, Tag2> {
    int val;
};

#define N 5

static void TestPushBack() {
    IntrusiveList<MyStruct, Tag1> list1;
    MyStruct s_vec[N];
    for (int i = 0; i < N; ++i) {
        s_vec[i].val = i;
    }

    for (int i = N - 1; i >= 0; --i) {
        list1.PushBack(&s_vec[i]);
    }

    assert(!list1.IsEmpty());

    int counter = N - 1;
    list1.ForEach([&counter](MyStruct* ms) -> void {
        assert(ms->val == counter);
        --counter;
    });

    counter = N - 1;
    list1.ForEachSafe([&counter](MyStruct* ms) -> void {
        assert(ms->val == counter);
        --counter;
        ms->IntrusiveListNode<MyStruct, Tag1>::Detach();
    });
}

static void TestPushFront() {
    IntrusiveList<MyStruct, Tag2> list2;
    MyStruct s_vec[N];
    for (int i = 0; i < N; ++i) {
        s_vec[i].val = i;
    }

    for (int i = N - 1; i >= 0; --i) {
        list2.PushFront(&s_vec[i]);
    }

    assert(!list2.IsEmpty());

    int counter = N - 1;
    list2.ForEachReverse([&counter](MyStruct* ms) -> void {
        assert(ms->val == counter);
        --counter;
    });

    counter = N - 1;
    list2.ForEachReverseSafe([&counter](MyStruct* ms) -> void {
        assert(ms->val == counter);
        --counter;
        ms->IntrusiveListNode<MyStruct, Tag2>::DetachWithoutReset();
    });
}

int main(void) {
    TestPushBack();
    TestPushFront();
    return 0;
}
