#pragma once

#include "types.hpp"

#include <cstdint>

namespace raijin
{
    class PriceLevel
    {
    public:
        void bind(OrderRef *orders, std::uint32_t capacity) noexcept
        {
            orders_ = orders;
            mask_ = capacity - 1;
        }

        bool push(OrderRef order) noexcept
        {
            if (count_ == mask_ + 1)
            {
                return false;
            }

            orders_[tail_] = order;
            tail_ = (tail_ + 1) & mask_;
            ++count_;
            return true;
        }

        OrderRef front() const noexcept
        {
            return orders_[head_];
        }

        void pop() noexcept
        {
            head_ = (head_ + 1) & mask_;
            --count_;
        }

        template <typename Predicate>
        void compact(Predicate is_active) noexcept
        {
            std::uint32_t active_count = 0;
            std::uint32_t current = head_;

            for (std::uint32_t i = 0; i < count_; ++i)
            {
                const OrderRef ref = orders_[current];

                if (is_active(ref))
                {
                    orders_[(head_ + active_count) & mask_] = ref;
                    ++active_count;
                }

                current = (current + 1) & mask_;
            }

            tail_ = (head_ + active_count) & mask_;
            count_ = active_count;
        }

        void clear() noexcept
        {
            head_ = 0;
            tail_ = 0;
            count_ = 0;
            total_volume_ = 0;
        }

        bool empty() const noexcept
        {
            return count_ == 0;
        }

        void add_volume(std::uint32_t volume) noexcept
        {
            total_volume_ += volume;
        }

        void remove_volume(std::uint32_t volume) noexcept
        {
            total_volume_ -= volume;
        }

        std::uint64_t total_volume() const noexcept
        {
            return total_volume_;
        }

    private:
        OrderRef *orders_ = nullptr;
        std::uint64_t total_volume_ = 0;
        std::uint32_t head_ = 0;
        std::uint32_t tail_ = 0;
        std::uint32_t count_ = 0;
        std::uint32_t mask_ = 0;
    };
}
