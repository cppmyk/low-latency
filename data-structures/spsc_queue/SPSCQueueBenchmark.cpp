#include <iostream>
#include <thread>
#include <chrono>
#include <stdexcept>
#include <vector>
#include "SPSCBlockingQueueBasic.h"
#include "SPSCBlockingQueuePowerOfTwo.h"
#include "SPSCBlockingQueueAligned.h"
#include "SPSCBlockingQueueCached.h"
#include "SPSCBlockingQueueMemoryOrders.h"

static constexpr int64_t kNumElements = 10'000'000;

template <typename Queue>
void ThroughputTest(const char* name) {
  Queue q;
  std::cout << "---------------------------------------" << std::endl;
  std::cout << "Throughput test: " << name << std::endl;

  std::thread consumer([&]() {
    int expected = 0;
    for (int64_t i = 0; i < kNumElements; ++i) {
      int val = q.Pop();
      if (val != expected++)
        throw std::runtime_error("value mismatch");
    }
  });

  auto start = std::chrono::steady_clock::now();
  for (int64_t i = 0; i < kNumElements; ++i) {
    q.Push(i);
  }
  consumer.join();
  auto stop = std::chrono::steady_clock::now();

  double ops_per_sec = double(kNumElements) /
      std::chrono::duration<double>(stop - start).count();
  std::cout << "Throughput: " << ops_per_sec / 1e6 << " Mops/sec" << std::endl;
}

int main() {
  // Order from least optimized to most optimized
  ThroughputTest<SPSCBlockingQueueBasic<int, 4096>>("SPSCBlockingQueueBasic");
  ThroughputTest<SPSCBlockingQueuePowerOfTwo<int, 4096>>("SPSCBlockingQueuePowerOfTwo");
  ThroughputTest<SPSCBlockingQueueAligned<int, 4096>>("SPSCBlockingQueueAligned");
  ThroughputTest<SPSCBlockingQueueCached<int, 4096>>("SPSCBlockingQueueCached");
  ThroughputTest<SPSCBlockingQueueMemoryOrders<int, 4096>>("SPSCBlockingQueueMemoryOrders");

  return 0;
}
