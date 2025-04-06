#ifndef __CPPUTILS_GUARD_H__
#define __CPPUTILS_GUARD_H__

#include <utility>

namespace cpputils {

template <typename FuncType>
class Guard final {
public:
    Guard(FuncType&& dtor) : m_dtor(std::forward<FuncType>(dtor)) {}
    ~Guard() {
        m_dtor();
    }

private:
    FuncType m_dtor;
};

}

#endif
