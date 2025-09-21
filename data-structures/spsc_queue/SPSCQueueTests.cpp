#include <gtest/gtest.h>
#include <thread>
#include "SPSCBlockingQueueBasic.h"
#include "SPSCBlockingQueuePowerOfTwo.h"
#include "SPSCBlockingQueueAligned.h"
#include "SPSCBlockingQueueCached.h"
#include "SPSCBlockingQueueMemoryOrders.h"

template <template <typename, size_t> class Q>
struct QueueWrapper {
  template <typename T, size_t N>
  using Queue = Q<T, N>;
};

using QueueTypes = ::testing::Types<
    QueueWrapper<SPSCBlockingQueueBasic>,
    QueueWrapper<SPSCBlockingQueuePowerOfTwo>,
    QueueWrapper<SPSCBlockingQueueAligned>,
    QueueWrapper<SPSCBlockingQueueCached>,
    QueueWrapper<SPSCBlockingQueueMemoryOrders>
>;

template <typename QueueWrapperT>
class SPSCQueueTest : public ::testing::Test {
 protected:
  template <size_t N>
  using Queue = typename QueueWrapperT::template Queue<int, N>;
};

TYPED_TEST_SUITE(SPSCQueueTest, QueueTypes);

TYPED_TEST(SPSCQueueTest, SingleThreaded) {
  typename TestFixture::template Queue<4> q;

  q.Push(42);
  EXPECT_EQ(q.Pop(), 42);

  q.Push(1);
  q.Push(2);
  q.Push(3);

  EXPECT_EQ(q.Pop(), 1);
  EXPECT_EQ(q.Pop(), 2);
  EXPECT_EQ(q.Pop(), 3);
}

TYPED_TEST(SPSCQueueTest, FifoOrder) {
  typename TestFixture::template Queue<16> q;

  for (int i = 0; i < 10; ++i) {
    q.Push(i);
  }

  for (int i = 0; i < 10; ++i) {
    EXPECT_EQ(q.Pop(), i);
  }
}

TYPED_TEST(SPSCQueueTest, MultiThreaded) {
  typename TestFixture::template Queue<128> q;
  constexpr int kNumItems = 1000;

  std::thread producer([&]() {
    for (int i = 0; i < kNumItems; ++i) {
      q.Push(i);
    }
  });

  std::thread consumer([&]() {
    for (int i = 0; i < kNumItems; ++i) {
      int value = q.Pop();
      EXPECT_GE(value, 0);
      EXPECT_LT(value, kNumItems);
    }
  });

  producer.join();
  consumer.join();
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
