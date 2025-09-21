#pragma once

#include <vector>
#include <atomic>

template<typename T, size_t capacity>
class SPSCBlockingQueueMemoryOrders {
  static_assert(std::has_single_bit(capacity), "Capacity must be equal to the power of 2");
  static constexpr size_t kMask = capacity - 1;
  static constexpr size_t kCacheLineSize = 64;

  struct Slot {
    alignas(kCacheLineSize) T value;
  };
 public:
  SPSCBlockingQueueMemoryOrders() : data_(capacity) {
  }

  void Push(T value) {
    size_t tail = tail_.load(std::memory_order_relaxed);
    size_t next_idx = Next(tail);

    while (next_idx == head_cache_) {
      head_cache_ = head_.load(std::memory_order_acquire);
      // Wait until consumer pops from the queue
    }

    data_[tail].value = std::move(value);
    tail_.store(next_idx, std::memory_order_release);
  }

  T Pop() {
    size_t head = head_.load(std::memory_order_relaxed);

    while (head == tail_cache_) {
      tail_cache_ = tail_.load(std::memory_order_acquire);
      // Wait until producer puts an element into the queue
    }

    T value = std::move(data_[head].value);
    head_.store(Next(head), std::memory_order_release);

    return value;
  }

 private:
  size_t Next(size_t idx) const {
    return (idx + 1) & kMask;
  }

  alignas(kCacheLineSize) std::vector<Slot> data_;

  alignas(kCacheLineSize) std::atomic<size_t> head_{0};
  alignas(kCacheLineSize) size_t tail_cache_{0};

  alignas(kCacheLineSize) std::atomic<size_t> tail_{0};
  alignas(kCacheLineSize) size_t head_cache_{0};
};