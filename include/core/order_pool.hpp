#pragma once

#include "types.hpp"
#include <memory_resource>
#include <vector>

namespace raijin
{
    class OrderPool
    {
    private:
        std::pmr::unsynchronized_pool_resource pool;
        std::pmr::monotonic_buffer_resource buffer;
        std::vector<std::byte> arena;

    public:
        explicit OrderPool(size_t capacity); // we call this constructor once at the start of the program for memory allcoation
        ~OrderPool() = default;

        // this is a safety measure to prevent copying
        OrderPool(const OrderPool &) = delete;
        OrderPool &operator=(const OrderPool &) = delete;

        Order *allocate();
        void deallocate(Order *order);
    };
}