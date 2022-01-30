cmake_minimum_required(VERSION 3.10)
project(cpputils)

# ----- options ----- #

option(CPPUTILS_BUILD_TESTS "build tests" ON)
option(CPPUTILS_INSTALL "install headers and libs" ON)

# ----- targets ----- #

file(GLOB CPPUTILS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_library(cpputils_static STATIC ${CPPUTILS_SRC})
target_include_directories(cpputils_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# ----- dependencies ----- #

if(NOT math_static)
    set(MATH_BUILD_TESTS OFF CACHE BOOL "")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../math ${CMAKE_CURRENT_BINARY_DIR}/math)
    target_link_libraries(cpputils_static PUBLIC math_static)
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
