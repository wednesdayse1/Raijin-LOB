#include "../../include/core/limit_order_book.hpp"
#include <cstdlib>
#include <new>

namespace raijin
{
    LimitOrderBook::LimitOrderBook(size_t pool_capacity) : pool(pool_capacity), best_bid_price(0), best_ask_price(MAX_PRICE_TICKS)
    {

        for (size_t i = 0; i < MAX_PRICE_TICKS; ++i)
        {
            bid_levels[i] = nullptr;
            ask_levels[i] = nullptr;
        }
    }
    LimitOrderBook::~LimitOrderBook()
    {
        for (size_t i=0; i < MAX_PRICE_TICKS; ++i)
        {
            if (bid_levels[i])
                std::free(bid_levels[i]);
            if (ask_levels[i])
                std::free(ask_levels[i]);
        }
    }

    PriceLevel *LimitOrderBook::get_or_create_level(uint64_t price, bool is_buy)
    {
        PriceLevel **levels = is_buy ? bid_levels : ask_levels;

        if (levels[price] == nullptr)
        {
            void *raw_memory = std::aligned_alloc(64, sizeof(PriceLevel));
            levels[price] = new (raw_memory) PriceLevel();
            levels[price]->price = price;
        }
        return levels[price];
    }

    void LimitOrderBook::add_order(uint64_t order_id, uint64_t price, uint32_t volume, bool is_buy)
    {
        PriceLevel *level = get_or_create_level(price, is_buy);

        Order *order = pool.allocate();
        if (!order)
            return;

        order->order_id = order_id;
        order->price = price;
        order->volume = volume;
        order->is_buy = is_buy;

        level->append_order(order);

        if (is_buy && price > best_bid_price)
        {
            best_bid_price = price;
        }
        else if (!is_buy && price < best_ask_price)
        {
            best_ask_price = price;
        }
    }
    void LimitOrderBook::cancel_order(Order *order)
    {
        bool is_buy = order->is_buy;
        uint64_t price = order->price;

        PriceLevel **levels = is_buy ? bid_levels : ask_levels;
        PriceLevel *level = levels[price];

        level->remove_order(order);

        pool.deallocate(order);
    }
    void LimitOrderBook::execute_order(Order *order, uint32_t execution_volume)
    {
        if (execution_volume >= order->volume)
        {
            cancel_order(order); // using cancel here cuz the memory op is identical to whats needed... settlement is done externally.. the goal of the lob is to eliminate the order thats it
        }
        else
        {
            order->volume -= execution_volume;
            PriceLevel **levels = order->is_buy ? bid_levels : ask_levels;
            levels[order->price]->total_volume -= execution_volume;
        }
    }
}