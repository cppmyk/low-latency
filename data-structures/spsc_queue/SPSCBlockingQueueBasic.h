#pragma once

#include <vector>
#include <atomic>

template<typename T, size_t capacity>
class SPSCBlockingQueueBasic {
 public:
  SPSCBlockingQueueBasic() : data_(capacity) {
  }

  void Push(T value) {
    size_t tail = tail_.load();
    size_t next_tail = Next(tail);

    while (next_tail == head_.load()) {
      // Wait until consumer pops from the queue
    }

    data_[tail] = std::move(value);
    tail_.store(next_tail);
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
    return (idx + 1) % capacity;
  }

  std::vector<T> data_;
  std::atomic<size_t> head_{0};
  std::atomic<size_t> tail_{0};
};