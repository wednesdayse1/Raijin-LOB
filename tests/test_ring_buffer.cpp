#include <gtest/gtest.h>
#include <../include/core/ring_buffer.hpp>
#include <thread>
#include <vector>

using namespace raijin;

TEST(RingBufferTest, BasicPushPop)
{
    RingBuffer<ExecutionReceipt> rb(4);
    ExecutionReceipt item;

    EXPECT_FALSE(rb.pop(item)); // buffer is empty
    EXPECT_TRUE(rb.push({100, 200, 5000, 50}));
    EXPECT_TRUE(rb.pop(item));
    EXPECT_EQ(item.maker_order_id, 100);
    EXPECT_EQ(item.executed_volume, 50);
    EXPECT_FALSE(rb.pop(item));
}

TEST(RingBufferTest, BufferFull)
{
    RingBuffer<ExecutionReceipt> rb(2);
    EXPECT_TRUE(rb.push({1, 2, 100, 10}));
    EXPECT_TRUE(rb.push({3, 4, 100, 10}));
    // 3rd push should fail because capacity is 2
    EXPECT_FALSE(rb.push({5, 6, 100, 10}));
}

TEST(RingBufferTest, InvalidCapacityThrows)
{
    EXPECT_THROW(RingBuffer<ExecutionReceipt> rb(3), std::invalid_argument);
}