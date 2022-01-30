cmake_minimum_required(VERSION 3.10)
project(cpputils-test)

add_executable(test_text_utils test_text_utils.cpp)
target_link_libraries(test_text_utils PRIVATE cpputils_static)

add_executable(test_skiplist test_skiplist.cpp)
target_link_libraries(test_skiplist PRIVATE cpputils_static)
