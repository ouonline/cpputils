cmake_minimum_required(VERSION 3.14)
project(cpputils)

option(CPPUTILS_BUILD_TESTS "build tests" ON)

file(GLOB CPPUTILS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_library(cpputils_static STATIC ${CPPUTILS_SRC})
target_include_directories(cpputils_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(cpputils_static PUBLIC cxx_std_11)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../math ${CMAKE_CURRENT_BINARY_DIR}/math)
target_link_libraries(cpputils_static PUBLIC math_static)

if(CPPUTILS_BUILD_TESTS)
    add_subdirectory(tests)
endif()
