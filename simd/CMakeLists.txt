cmake_minimum_required(VERSION 3.29)

add_executable(orderbook_snapshot orderbook_snapshot.cpp)
target_link_libraries(orderbook_snapshot PRIVATE benchmark::benchmark Threads::Threads)
target_compile_options(orderbook_snapshot PRIVATE -O2)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")