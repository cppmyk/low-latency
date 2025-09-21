#include <benchmark/benchmark.h>
#include <thread>
#include <stdexcept>
#include "SPSCBlockingQueueBasic.h"
#include "SPSCBlockingQueuePowerOfTwo.h"
#include "SPSCBlockingQueueAligned.h"
#include "SPSCBlockingQueueCached.h"
#include "SPSCBlockingQueueMemoryOrders.h"

static constexpr int64_t kNumElements = 10'000'000;

template <typename Queue>
static void BM_SPSCQueueThroughput(benchmark::State& state) {
  for (auto _ : state) {
    Queue q;

    std::thread consumer([&]() {
      int expected = 0;
      for (int64_t i = 0; i < kNumElements; ++i) {
        int val = q.Pop();
        if (val != expected++)
          throw std::runtime_error("value mismatch");
      }
    });

    for (int64_t i = 0; i < kNumElements; ++i) {
      q.Push(i);
    }

    consumer.join();
  }
  state.SetItemsProcessed(kNumElements * state.iterations());
}

BENCHMARK_TEMPLATE(BM_SPSCQueueThroughput, SPSCBlockingQueueBasic<int, 4096>)
    ->Unit(benchmark::kMillisecond)
    ->MinTime(3.0);

BENCHMARK_TEMPLATE(BM_SPSCQueueThroughput, SPSCBlockingQueuePowerOfTwo<int, 4096>)
    ->Unit(benchmark::kMillisecond)
    ->MinTime(3.0);

BENCHMARK_TEMPLATE(BM_SPSCQueueThroughput, SPSCBlockingQueueAligned<int, 4096>)
    ->Unit(benchmark::kMillisecond)
    ->MinTime(3.0);

BENCHMARK_TEMPLATE(BM_SPSCQueueThroughput, SPSCBlockingQueueCached<int, 4096>)
    ->Unit(benchmark::kMillisecond)
    ->MinTime(3.0);

BENCHMARK_TEMPLATE(BM_SPSCQueueThroughput, SPSCBlockingQueueMemoryOrders<int, 4096>)
    ->Unit(benchmark::kMillisecond)
    ->MinTime(3.0);

BENCHMARK_MAIN();
