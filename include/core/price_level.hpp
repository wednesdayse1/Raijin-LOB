#pragma once

#include "types.hpp"

namespace raijin
{

    struct alignas(64) PriceLevel
    {
        uint64_t price;
        uint32_t total_volume;

        Order *head;
        Order *tail;

        PriceLevel *next;
        PriceLevel *prev;

        PriceLevel() : price(0), total_volume(0), head(nullptr), tail(nullptr), next(nullptr), prev(nullptr) {}

        inline void append_order(Order *order)
        {
            order->next = nullptr;
            order->prev = tail;

            if (tail == nullptr)
            {
                head = order;
                tail = order;
            }
            else
            {
                tail->next = order;
                tail = order;
            }
            total_volume += order->volume;
        }

        inline void remove_order(Order *order)
        {
            if (order->prev)
            {
                order->prev->next = order->next;
            }
            else
            {
                head = order->next;
            }
            if (order->next != nullptr)
            {
                order->next->prev = order->prev;
            }
            else
            {
                tail = order->prev;
            }

            order->next = nullptr;
            order->prev = nullptr;

            total_volume -= order->volume;
        }
    };
}