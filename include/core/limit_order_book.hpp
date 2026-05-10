#pragma once

#include "types.hpp"
#include "price_level.hpp"
#include "order_pool.hpp"
#include <cstdint>

namespace raijin
{
    class LimitOrderBook
    {
    private:
        OrderPool pool;
        static constexpr size_t MAX_PRICE_TICKS = 100000;
        PriceLevel *bid_levels[MAX_PRICE_TICKS];
        PriceLevel *ask_levels[MAX_PRICE_TICKS];

        uint64_t best_bid_price;
        uint64_t best_ask_price;

        PriceLevel *get_or_create_level(uint64_t price, bool is_buy);
        void process_order(Order &order);

    public:
        explicit LimitOrderBook(size_t pool_capacity);
        ~LimitOrderBook();

        void add_order(uint64_t order_id, uint64_t price, uint32_t volume, bool is_buy);
        void cancel_order(Order *order);
        void execute_order(Order *order, uint32_t execution_volume);
        void match_buy_order(Order *incoming_buy);
        void match_sell_order(Order *incoming_sell);
    };
}