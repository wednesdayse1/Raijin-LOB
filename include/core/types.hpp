#pragma once

#include <cstdint>

namespace raijin
{
    struct alignas(64) Order // alignas(64) so we can fit the struct exactly into the cpu cache line, prevents cache misses, and makes the memory access blazingly fast.
    {
        uint64_t order_id;
        uint64_t price;
        uint32_t volume;
        bool is_buy;

        Order *next;
        Order *prev;
    };
}