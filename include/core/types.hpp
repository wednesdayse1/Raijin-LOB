#pragma once

#include <cstdint>

namespace raijin
{
    struct alignas(16) Order
    {
        uint64_t order_id;
        uint32_t price_tick;
        uint32_t volume;
    };
}