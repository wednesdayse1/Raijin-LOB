#include <gtest/gtest.h>
#include "../include/core/limit_order_book.hpp"

using namespace raijin;

class LimitOrderBookTest : public ::testing::Test
{
protected:
    BookConfig config{
        .order_pool_capacity = 1000,
        .price_level_count = 100000,
        .level_queue_capacity = 256,
        .max_order_id = 1000};

    std::unique_ptr<LimitOrderBook> book;

    void SetUp() override
    {
        book = std::make_unique<LimitOrderBook>(config);
    }
};

TEST_F(LimitOrderBookTest, AddSingleBuyOrder)
{
    bool success = book->add_order(1, 5000, 100, true);
    EXPECT_TRUE(success);
    EXPECT_EQ(book->best_bid_tick(), 5000);
    EXPECT_EQ(book->bid_volume(5000), 100);

    EXPECT_EQ(book->best_ask_tick(), UINT32_MAX);
}

TEST_F(LimitOrderBookTest, AddSingleSellOrder)
{
    bool success = book->add_order(2, 5010, 50, false);
    EXPECT_TRUE(success);
    EXPECT_EQ(book->best_ask_tick(), 5010);
    EXPECT_EQ(book->ask_volume(5010), 50);
    EXPECT_EQ(book->best_bid_tick(), UINT32_MAX);
}

TEST_F(LimitOrderBookTest, MatchFullFill)
{
    book->add_order(1, 5000, 100, false);
    EXPECT_EQ(book->ask_volume(5000), 100);

    book->add_order(2, 5000, 100, true);
    EXPECT_EQ(book->ask_volume(5000), 0);
    EXPECT_EQ(book->bid_volume(5000), 0);
    EXPECT_EQ(book->best_ask_tick(), UINT32_MAX);
}

TEST_F(LimitOrderBookTest, MatchPartialFillIncomingSmaller)
{
    book->add_order(1, 5000, 100, false);
    book->add_order(2, 5000, 40, true);
    EXPECT_EQ(book->ask_volume(5000), 60);
    EXPECT_EQ(book->bid_volume(5000), 0);
    EXPECT_EQ(book->best_ask_tick(), 5000);
}

TEST_F(LimitOrderBookTest, CancelOrder)
{
    book->add_order(1, 5000, 100, true);
    EXPECT_EQ(book->bid_volume(5000), 100);
    bool success = book->cancel_order(1);
    EXPECT_TRUE(success);
    EXPECT_EQ(book->bid_volume(5000), 0);
    EXPECT_EQ(book->best_bid_tick(), UINT32_MAX);
}

TEST_F(LimitOrderBookTest, MatchPartialFIllIncomingLarger)
{
    book->add_order(1, 5000, 100, false);
    book->add_order(2, 5000, 150, true);
    EXPECT_EQ(book->ask_volume(5000), 0);
    EXPECT_EQ(book->bid_volume(5000), 50);
    EXPECT_EQ(book->best_ask_tick(), UINT32_MAX);
}

// spoofing test
TEST_F(LimitOrderBookTest, ToxicFlowCompaction)
{
    for (uint64_t i = 1; i <= 256; i++)
    {
        book->add_order(i, 5000, 10, true);
    }
    for (uint64_t i = 1; i <= 100; i++)
    {
        book->cancel_order(i);
    }
    bool success = book->add_order(257, 5000, 10, true);
    EXPECT_TRUE(success);
    EXPECT_EQ(book->bid_volume(5000), 1570);
}