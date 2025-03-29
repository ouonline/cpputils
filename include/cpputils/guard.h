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

private:
    Guard(Guard&&) = delete;
    Guard(const Guard&) = delete;
    Guard& operator=(Guard&&) = delete;
    Guard& operator=(const Guard&) = delete;
};

}

#endif
