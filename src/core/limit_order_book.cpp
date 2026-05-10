#include "../../include/core/limit_order_book.hpp"
#include <cstdlib>
#include <new>
#include <algorithm>

namespace raijin
{
    LimitOrderBook::LimitOrderBook(size_t pool_capacity) : pool(pool_capacity), best_bid_price(MAX_PRICE_TICKS), best_ask_price(MAX_PRICE_TICKS)
    {

        for (size_t i = 0; i < MAX_PRICE_TICKS; ++i)
        {
            bid_levels[i] = nullptr;
            ask_levels[i] = nullptr;
        }
    }
    LimitOrderBook::~LimitOrderBook()
    {
        for (size_t i = 0; i < MAX_PRICE_TICKS; ++i)
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

        if (is_buy && (best_bid_price == MAX_PRICE_TICKS || price > best_bid_price))
        {
            best_bid_price = price;
        }
        else if (!is_buy && (best_ask_price == MAX_PRICE_TICKS || price < best_ask_price))
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
    void LimitOrderBook::match_buy_order(Order *incoming_buy)
    {
        while (incoming_buy->volume > 0)
        {
            while (best_ask_price < MAX_PRICE_TICKS && (ask_levels[best_ask_price] == nullptr || ask_levels[best_ask_price]->head == nullptr))
            {
                ++best_ask_price;
            }

            if (best_ask_price >= MAX_PRICE_TICKS || incoming_buy->price < best_ask_price)
            {
                break;
            }

            PriceLevel *level = ask_levels[best_ask_price];
            Order *resting_sell = level->head;
            uint32_t fill_volume = std::min(incoming_buy->volume, resting_sell->volume);
            incoming_buy->volume -= fill_volume;
            execute_order(resting_sell, fill_volume);
            if (level->head == nullptr)
            {
                ++best_ask_price;
            }
        }
    }

    void LimitOrderBook::match_sell_order(Order *incoming_sell)
    {
        while (incoming_sell->volume > 0)
        {
            while (best_bid_price < MAX_PRICE_TICKS && (bid_levels[best_bid_price] == nullptr || bid_levels[best_bid_price]->head == nullptr))
            {
                if (best_bid_price == 0)
                {
                    best_bid_price = MAX_PRICE_TICKS;
                    break;
                }

                --best_bid_price;
            }

            if (best_bid_price >= MAX_PRICE_TICKS || incoming_sell->price > best_bid_price)
            {
                break;
            }

            PriceLevel *level = bid_levels[best_bid_price];
            Order *resting_buy = level->head;
            uint32_t fill_volume = std::min(incoming_sell->volume, resting_buy->volume);
            incoming_sell->volume -= fill_volume;
            execute_order(resting_buy, fill_volume);
            if (level->head == nullptr)
            {
                if (best_bid_price == 0)
                {
                    best_bid_price = MAX_PRICE_TICKS;
                }
                else
                {
                    --best_bid_price;
                }
            }
        }
    }
    void LimitOrderBook::process_order(Order &order)
    {
        if (order.is_buy)
        {
            match_buy_order(&order);
        }
        else
        {
            match_sell_order(&order);
        }

        if (order.volume > 0)
        {
            add_order(order.order_id, order.price, order.volume, order.is_buy);
        }
    }
}