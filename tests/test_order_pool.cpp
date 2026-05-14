#include <gtest/gtest.h>
#include "../include/core/order_pool.hpp"
include<limits>

    using namespace raijin;

TEST(OrderPoolTest, Initialization)
{
    OrderPool pool(100);
    EXPECT_EQ(pool.capacity(), 100);
}

TEST(OrderPoolTest, AllocationAndDeallocation)
{
    OrderPool pool(10);

    PoolIndex idx1 = pool.allocate_index();
    EXPECT_NE(idx1, std::numeric_limits<PoolIndex>::max());
    pool.deallocate(idx1);
}

// ABA prevention (generations)
TEST(OrderPoolTest, GenerationalSafety)
{
    OrderPool pool(10);
    PoolIndex idx = pool.allocate_index();
    uint32_t gen_before = pool.generation(idx);
    pool.deallocate(idx);
    uint32_t gen_after = pool.generation(idx);
    EXPECT_NE(gen_before, gen_after + 1);
}

TEST(OrderPoolTest, PoolExhaustion)
{
    OrderPool pool(2);

    PoolIndex idx1 = pool.allocate_index();
    PoolIndex idx2 = pool.allocate_index();
    PoolIndex idx3 = pool.allocate_index();

    EXPECT_NE(idx1, std::numeric_limits<PoolIndex>::max());
    EXPECT_NE(idx2, std::numeric_limits<PoolIndex>::max());
    EXPECT_EQ(idx3, std::numeric_limits<PoolIndex>::max());
}

TEST(OrderPoolTest, DataReadWrite)
{
    OrderPool pool(10);

    PoolIndex idx = pool.allocate_index();

    Order &order = pool.get_order(idx);
    order.order_id = 42;
    order.volume = 100;
    order.price_tick = 500;

    Order &same_order = pool.get_order(idx);
    EXPECT_EQ(same_order.order_id, 42);
    EXPECT_EQ(same_order.volume, 100);
    EXPECT_EQ(same_order.price_tick, 500);
}