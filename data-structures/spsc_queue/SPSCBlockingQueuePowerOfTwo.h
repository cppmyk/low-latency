#pragma once

#include <vector>
#include <atomic>

template<typename T, size_t capacity>
//requires (std::has_single_bit(size))
class SPSCBlockingQueuePowerOfTwo {
  static_assert(std::has_single_bit(capacity), "Capacity must be equal to the power of 2");
  static constexpr size_t kMask = capacity - 1;
 public:
  SPSCBlockingQueuePowerOfTwo() : data_(capacity) {
  }

  void Push(T value) {
    size_t tail = tail_.load();
    size_t next_idx = Next(tail);

    while (next_idx == head_.load()) {
      // Wait until consumer pops from the queue
    }

    data_[tail] = std::move(value);
    tail_.store(next_idx);
  }

  T Pop() {
    size_t head = head_.load();

    while (head == tail_.load()) {
      // Wait until producer puts an element into the queue
    }

    T value = std::move(data_[head]);
    head_.store(Next(head));
    return value;
  }

 private:
  size_t Next(size_t idx) const {
    return (idx + 1) & kMask;
  }

  std::vector<T> data_;
  std::atomic<size_t> head_{0};
  std::atomic<size_t> tail_{0};
};