#include <benchmark/benchmark.h>
#include <array>
#include <utility>
#include <iostream>

#include <arm_neon.h>

// Array of structures
struct OrderbookUpdate {
  double askPrice;
  double askVolume;
  double bidPrice;
  double bidVolume;
};

// Structure of arrays
struct OrderbookSnapshot {
  std::array<double, 30> askPrices;
  std::array<double, 30> askVolumes;
  std::array<double, 30> bidPrices;
  std::array<double, 30> bidVolumes;
};

std::pair<double, double> FillSnapshot(OrderbookSnapshot& snapshot) {
  for (int i = 0; i < 30; i++) {
    snapshot.askPrices[i] = 100.0 + i;
    snapshot.askVolumes[i] = 1.0;
    snapshot.bidPrices[i] = 90.0 + i;
    snapshot.bidVolumes[i] = 2.0;
  }

  double expectedAsk = 3435.0;
  double expectedBid = 6270.0;

  return {expectedAsk, expectedBid};
}

std::pair<double, double> FillSnapshot(std::array<OrderbookUpdate, 30>& snapshot) {
  for (int i = 0; i < 30; i++) {
    snapshot[i].askPrice = 100.0 + i;
    snapshot[i].askVolume = 1.0;
    snapshot[i].bidPrice = 90.0 + i;
    snapshot[i].bidVolume = 2.0;
  }

  double expectedAsk = 3435.0;
  double expectedBid = 6270.0;

  return {expectedAsk, expectedBid};
}

static void ArrayOfStructuresBench(benchmark::State& state) {
  std::array<OrderbookUpdate, 30> snapshot{};
  auto [expected_ask_sum, expected_bid_sum] = FillSnapshot(snapshot);
  double ask_sum = 0;
  double bid_sum = 0;

  for (auto _ : state) {
    ask_sum = 0;
    bid_sum = 0;

    for (size_t i = 0; i < 30; ++i) {
      ask_sum += snapshot[i].askPrice * snapshot[i].askVolume;
      bid_sum += snapshot[i].bidPrice * snapshot[i].bidVolume;
    }

    benchmark::DoNotOptimize(ask_sum);
    benchmark::DoNotOptimize(bid_sum);
  }
  state.SetItemsProcessed(30 * state.iterations());

  if (ask_sum != expected_ask_sum || bid_sum != expected_bid_sum) {
    std::cerr << "Incorrect calculations" << std::endl;
  }
}

static void StructureOfArraysBench(benchmark::State& state) {
  OrderbookSnapshot snapshot{};
  auto [expected_ask_sum, expected_bid_sum] = FillSnapshot(snapshot);
  double ask_sum = 0;
  double bid_sum = 0;

  for (auto _ : state) {
    ask_sum = 0;
    bid_sum = 0;

    for (size_t i = 0; i < 30; ++i) {
      ask_sum += snapshot.askPrices[i] * snapshot.askVolumes[i];
      bid_sum += snapshot.bidPrices[i] * snapshot.bidVolumes[i];
    }

    benchmark::DoNotOptimize(ask_sum);
    benchmark::DoNotOptimize(bid_sum);
  }
  state.SetItemsProcessed(30 * state.iterations());

  if (ask_sum != expected_ask_sum || bid_sum != expected_bid_sum) {
    std::cerr << "Incorrect calculations" << std::endl;
  }
}

static void StructureOfArraysIntrinsicsBench(benchmark::State& state) {
  OrderbookSnapshot snapshot{};
  auto [expected_ask_sum, expected_bid_sum] = FillSnapshot(snapshot);
  double ask_sum = 0;
  double bid_sum = 0;

  for (auto _ : state) {
    ask_sum = 0;
    bid_sum = 0;

    float64x2_t ask_acc = vdupq_n_f64(0.0);
    float64x2_t bid_acc = vdupq_n_f64(0.0);

    size_t i = 0;
    for (; i + 1 < 30; i += 2) {
      float64x2_t askP = vld1q_f64(&snapshot.askPrices[i]);
      float64x2_t askV = vld1q_f64(&snapshot.askVolumes[i]);
      float64x2_t bidP = vld1q_f64(&snapshot.bidPrices[i]);
      float64x2_t bidV = vld1q_f64(&snapshot.bidVolumes[i]);

      ask_acc = vmlaq_f64(ask_acc, askP, askV);
      bid_acc = vmlaq_f64(bid_acc, bidP, bidV);
    }

    double ask_tmp[2], bid_tmp[2];
    vst1q_f64(ask_tmp, ask_acc);
    vst1q_f64(bid_tmp, bid_acc);

    ask_sum = ask_tmp[0] + ask_tmp[1];
    bid_sum = bid_tmp[0] + bid_tmp[1];

    benchmark::DoNotOptimize(ask_sum);
    benchmark::DoNotOptimize(bid_sum);
  }
  state.SetItemsProcessed(30 * state.iterations());

  if (ask_sum != expected_ask_sum || bid_sum != expected_bid_sum) {
    std::cerr << "Incorrect calculations" << std::endl;
  }
}

BENCHMARK(StructureOfArraysBench);
BENCHMARK(ArrayOfStructuresBench);
BENCHMARK(StructureOfArraysIntrinsicsBench);

BENCHMARK_MAIN();