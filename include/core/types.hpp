#pragma once

#include <cstdint>

namespace raijin
{
    struct Order
    {
        uint64_t order_id;
        uint64_t price_tick;
        uint32_t volume;
    };
}