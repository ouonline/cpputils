cmake_minimum_required(VERSION 3.14)
project(cpputils)

# ----- options ----- #

option(CPPUTILS_BUILD_TESTS "build tests" ON)
option(CPPUTILS_INSTALL "install headers and libs" ON)

# ----- targets ----- #

file(GLOB_RECURSE __CPPUTILS_SRC__ ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_library(cpputils_static STATIC ${__CPPUTILS_SRC__})
unset(__CPPUTILS_SRC__)

target_include_directories(cpputils_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(cpputils_static PUBLIC cxx_std_11)

if(MSVC)
    target_compile_options(cpputils_static PRIVATE /W4)
else()
    target_compile_options(cpputils_static PRIVATE -Wall -Wextra -Werror)
endif()

# ----- dependencies ----- #

include(FetchContent)
set(FETCHCONTENT_QUIET OFF)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)

if(NOT CPPUTILS_DEPS_DIR)
    get_filename_component(CPPUTILS_DEPS_DIR ${CMAKE_CURRENT_BINARY_DIR}/deps ABSOLUTE)
    message("-- `CPPUTILS_DEPS_DIR` is not set. set `CPPUTILS_DEPS_DIR` to `${CPPUTILS_DEPS_DIR}`.")
endif()

if(NOT TARGET cutils_static)
    set(CUTILS_BUILD_TESTS OFF CACHE BOOL "")
    set(CUTILS_DEPS_DIR ${CPPUTILS_DEPS_DIR})
    FetchContent_Declare(cutils
        GIT_REPOSITORY "https://github.com/ouonline/cutils.git"
        GIT_TAG master
        SOURCE_DIR ${CPPUTILS_DEPS_DIR}/cutils
        BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/cutils-build
        SUBBUILD_DIR ${CPPUTILS_DEPS_DIR}/cutils-subbuild)
    FetchContent_MakeAvailable(cutils)
endif()

target_link_libraries(cpputils_static PUBLIC cutils_static)

# ----- installations ----- #

if(CPPUTILS_INSTALL)
    install(DIRECTORY include DESTINATION .)
    install(TARGETS cpputils_static DESTINATION lib)
endif()

# ----- tests ----- #

if(CPPUTILS_BUILD_TESTS)
    add_subdirectory(tests)
endif()
