cmake_minimum_required(VERSION 3.29)

#add_compile_options(-fsanitize=thread)
#add_link_options(-fsanitize=thread)

add_executable(spsc_queue_google_benchmark SPSCQueueGoogleBenchmark.cpp)
target_link_libraries(spsc_queue_google_benchmark PRIVATE benchmark::benchmark Threads::Threads)

add_executable(spsc_queue_benchmark SPSCQueueBenchmark.cpp)

add_executable(queue_tests SPSCQueueTests.cpp)
target_link_libraries(queue_tests PRIVATE GTest::gtest_main GTest::gtest GTest::gmock)

include(GoogleTest)
gtest_discover_tests(queue_tests)