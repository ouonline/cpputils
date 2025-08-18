#include "cpputils/ring_buffer.h"
using namespace std;
using namespace cpputils;

#undef NDEBUG
#include <assert.h>

int main(void) {
    RingBuffer<int> cb(2);
    assert(cb.size() == 0);
    assert(cb.IsEmpty());

    cb.PushBack(10);
    assert(cb.size() == 1);
    assert(cb.front() == 10);
    assert(cb.back() == 10);

    cb.PushBack(20);
    assert(cb.size() == 2);
    assert(cb.front() == 10);
    assert(cb.back() == 20);

    cb.PushBack(30);
    assert(cb.size() == 2);
    assert(cb.front() == 20);
    assert(cb.back() == 30);

    cb.PushBack(40);
    assert(cb.size() == 2);
    assert(cb.front() == 30);
    assert(cb.back() == 40);
    assert(!cb.IsEmpty());

    cb.Clear();
    assert(cb.IsEmpty());

    return 0;
}
