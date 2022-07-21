cmake_minimum_required(VERSION 3.10)
project(cpputils-test)

set (CMAKE_CXX_STANDARD 11)

add_executable(test_string_utils test_string_utils.cpp)
target_link_libraries(test_string_utils PRIVATE cpputils_static)

add_executable(test_skiplist test_skiplist.cpp)
target_link_libraries(test_skiplist PRIVATE cpputils_static)

add_executable(test_compact_memory_manager test_compact_memory_manager.cpp)
target_link_libraries(test_compact_memory_manager PRIVATE cpputils_static)

add_executable(test_robin_hood_hash_mem test_robin_hood_hash_mem.cpp)
target_include_directories(test_robin_hood_hash_mem PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)

add_executable(test_robin_hood_hash_pod test_robin_hood_hash_pod.cpp)
target_include_directories(test_robin_hood_hash_pod PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)

add_executable(test_robin_hood_hash_perf test_robin_hood_hash_perf.cpp)
target_include_directories(test_robin_hood_hash_perf PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
