#include "../../include/core/order_pool.hpp"

#include <limits>
#include <stdexcept>

namespace raijin
{
    std::size_t OrderPool::arena_bytes(std::size_t capacity)
    {
        constexpr std::size_t bytes_per_order = sizeof(Order) + sizeof(PoolIndex) + sizeof(std::uint32_t);
        constexpr std::size_t padding = alignof(Order) * 8;

        if (capacity == 0 || capacity > std::numeric_limits<PoolIndex>::max())
        {
            throw std::invalid_argument("invalid order pool capacity");
        }

        if (capacity > (std::numeric_limits<std::size_t>::max() - padding) / bytes_per_order)
        {
            throw std::invalid_argument("order pool arena overflow");
        }

        return capacity * bytes_per_order + padding;
    }

    OrderPool::OrderPool(std::size_t capacity)
        : arena_(arena_bytes(capacity)),
          memory_(arena_.data(), arena_.size(), std::pmr::null_memory_resource()),
          orders_(capacity, &memory_),
          free_(capacity, &memory_),
          generations_(capacity, 1, &memory_),
          free_count_(capacity)
    {
        for (std::size_t i = 0; i < capacity; ++i)
        {
            free_[i] = static_cast<PoolIndex>(capacity - 1 - i);
        }
    }

    PoolIndex OrderPool::allocate_index() noexcept
    {
        if (free_count_ == 0)
        {
            return std::numeric_limits<PoolIndex>::max();
        }

        return free_[--free_count_];
    }

    void OrderPool::deallocate(PoolIndex index) noexcept
    {
        ++generations_[index];
        free_[free_count_++] = index;
    }

    Order &OrderPool::get_order(PoolIndex index) noexcept
    {
        return orders_[index];
    }

    const Order &OrderPool::get_order(PoolIndex index) const noexcept
    {
        return orders_[index];
    }

    std::uint32_t OrderPool::generation(PoolIndex index) const noexcept
    {
        return generations_[index];
    }

    std::size_t OrderPool::capacity() const noexcept
    {
        return orders_.size();
    }
}
