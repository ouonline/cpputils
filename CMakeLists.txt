cmake_minimum_required(VERSION 3.10)
project(cpputils)

# ----- options ----- #

option(CPPUTILS_BUILD_TESTS "build tests" ON)
option(CPPUTILS_INSTALL "install headers and libs" ON)

# ----- targets ----- #

file(GLOB CPPUTILS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_library(cpputils_static STATIC ${CPPUTILS_SRC})
target_include_directories(cpputils_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(cpputils_static PRIVATE cxx_std_11)

# ----- dependencies ----- #

if(NOT CPPUTILS_DEPS_DIR)
    set(CPPUTILS_DEPS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)
    message(WARNING "`CPPUTILS_DEPS_DIR` is not set. set `CPPUTILS_DEPS_DIR` to `${CPPUTILS_DEPS_DIR}`.")
endif()

if(NOT TARGET math_static)
    set(MATH_BUILD_TESTS OFF CACHE BOOL "")
    set(MATH_DEPS_DIR ${CPPUTILS_DEPS_DIR})
    add_subdirectory(${CPPUTILS_DEPS_DIR}/math ${CMAKE_CURRENT_BINARY_DIR}/math-build)
endif()

target_link_libraries(cpputils_static PUBLIC math_static)

# ----- installations ----- #

if(CPPUTILS_INSTALL)
    install(DIRECTORY include DESTINATION .)
    install(TARGETS cpputils_static DESTINATION lib)
endif()

# ----- tests ----- #

if(CPPUTILS_BUILD_TESTS)
    add_subdirectory(tests)
endif()
