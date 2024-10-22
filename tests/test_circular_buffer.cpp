#include "cpputils/circular_buffer.h"
using namespace std;
using namespace cpputils;

#undef NDEBUG
#include <assert.h>

int main(void) {
    CircularBuffer<int> cb(2);
    assert(cb.Size() == 0);

    cb.PushBack(10);
    assert(cb.Size() == 1);
    assert(cb.Front() == 10);
    assert(cb.Back() == 10);

    cb.PushBack(20);
    assert(cb.Size() == 2);
    assert(cb.Front() == 10);
    assert(cb.Back() == 20);

    cb.PushBack(30);
    assert(cb.Size() == 2);
    assert(cb.Front() == 20);
    assert(cb.Back() == 30);

    cb.PushBack(40);
    assert(cb.Size() == 2);
    assert(cb.Front() == 30);
    assert(cb.Back() == 40);

    return 0;
}
