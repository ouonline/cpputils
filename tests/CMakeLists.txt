cmake_minimum_required(VERSION 3.10)
project(cpputils-test)

set (CMAKE_CXX_STANDARD 11)

add_executable(test_string_utils test_string_utils.cpp)
target_link_libraries(test_string_utils PRIVATE cpputils_static)

add_executable(test_skiplist test_skiplist.cpp)
target_link_libraries(test_skiplist PRIVATE cpputils_static)

add_executable(test_compact_addr_manager test_compact_addr_manager.cpp)
target_link_libraries(test_compact_addr_manager PRIVATE cpputils_static)

add_executable(test_ring_buffer test_ring_buffer.cpp)
target_link_libraries(test_ring_buffer PRIVATE cpputils_static)
