cmake_minimum_required(VERSION 3.29)
project(low_latency)

set(CMAKE_CXX_STANDARD 20)

find_package(benchmark REQUIRED)
find_package(Threads REQUIRED)

enable_testing()
find_package(GTest REQUIRED)

add_subdirectory(simd)
add_subdirectory(data-structures)

include(GoogleTest)
gtest_discover_tests(queue_tests)