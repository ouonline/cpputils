cmake_minimum_required(VERSION 3.14)
project(cpputils)

# ----- options ----- #

option(CPPUTILS_BUILD_TESTS "build tests" ON)
option(CPPUTILS_INSTALL "install headers and libs" ON)
option(CPPUTILS_HOLD_DEPS "do not update existing deps" OFF)

if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 14)
endif()

# ----- dependencies ----- #

include(FetchContent)
set(FETCHCONTENT_QUIET OFF)
set(FETCHCONTENT_UPDATES_DISCONNECTED ${CPPUTILS_HOLD_DEPS})

if(NOT CPPUTILS_DEPS_DIR)
    get_filename_component(CPPUTILS_DEPS_DIR ${CMAKE_CURRENT_BINARY_DIR}/deps ABSOLUTE)
    message(STATUS "`CPPUTILS_DEPS_DIR` is not set. set `CPPUTILS_DEPS_DIR` to `${CPPUTILS_DEPS_DIR}` by default.")
endif()

macro(cpputils_import_git_dep name repo commit)
    FetchContent_Declare(${name}
        GIT_REPOSITORY ${repo}
        GIT_TAG ${commit}
        GIT_SHALLOW TRUE
        SOURCE_DIR ${CPPUTILS_DEPS_DIR}/${name}
        BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/${name}-build
        SUBBUILD_DIR ${CPPUTILS_DEPS_DIR}/${name}-subbuild)
    FetchContent_MakeAvailable(${name})
endmacro()

if(NOT TARGET cutils_static)
    set(CUTILS_DEPS_DIR ${CPPUTILS_DEPS_DIR})
    set(CUTILS_BUILD_TESTS OFF)
    set(CUTILS_INSTALL ${CPPUTILS_INSTALL})
    set(CUTILS_HOLD_DEPS ${CPPUTILS_HOLD_DEPS})
    if(CPPUTILS_DEP_CUTILS_SOURCE_DIR)
        add_subdirectory(${CPPUTILS_DEP_CUTILS_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/cutils-build)
        set(cutils_SOURCE_DIR ${CPPUTILS_DEP_CUTILS_SOURCE_DIR})
    else()
        cpputils_import_git_dep(cutils "https://github.com/ouonline/cutils.git" master)
    endif()
endif()

# ----- targets ----- #

file(GLOB __CPPUTILS_SRC__ ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_library(cpputils_static STATIC ${__CPPUTILS_SRC__})
unset(__CPPUTILS_SRC__)

target_include_directories(cpputils_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(cpputils_static PUBLIC cutils_static)

if(MSVC)
    target_compile_options(cpputils_static PRIVATE /W4)
else()
    target_compile_options(cpputils_static PRIVATE -Wall -Wextra -Werror)
endif()

# ----- installations ----- #

if(CPPUTILS_INSTALL)
    install(DIRECTORY include DESTINATION .)
    install(TARGETS cpputils_static DESTINATION lib)
endif()

# ----- tests ----- #

if(CPPUTILS_BUILD_TESTS)
    add_subdirectory(tests)
endif()
