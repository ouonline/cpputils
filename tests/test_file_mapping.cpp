#include "cpputils/file_mapping.h"
using namespace std;
using namespace cpputils;

#undef NDEBUG
#include <assert.h>

static void test_init() {
    {
        FileMapping fm;
        assert(fm.Init(__FILE__, FileMapping::READ));
    }
    {
        FileMapping fm;
        assert(!fm.Init("nonexist",FileMapping::READ));
    }
}

static void test_offset_and_length() {
    FileMapping fm;
    const uint64_t offset = 22;
    const uint64_t length = 3;
    assert(fm.Init(__FILE__, FileMapping::READ, offset, length));
    assert(fm.size() == length);
    assert(fm.data());

    auto data = (const char*)fm.data();
    assert(data[0] == 'e');
    assert(data[1] == '_');
    assert(data[2] == 'm');
}

int main(void) {
    test_init();
    test_offset_and_length();
    return 0;
}
