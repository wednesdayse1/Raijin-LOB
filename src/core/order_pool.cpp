#include "../../include/core/order_pool.hpp"
#include <cstdlib>
#include <new>

namespace raijin
{
    OrderPool::OrderPool(size_t cap) : capacity(cap)
    {
        size_t total_bytes = capacity * sizeof(Order);
        arena = static_cast<Order *>(std::aligned_alloc(64, total_bytes));

        if (!arena)
        {
            throw std::bad_alloc();
        }
        for (size_t i = 0; i < capacity - 1; ++i)
        {
            arena[i].next = &arena[i + 1];
        }
        arena[capacity - 1].next = nullptr;
        free_head = &arena[0];
    }
    OrderPool::~OrderPool()
    {
        std::free(arena);
    }
    Order *OrderPool::allocate()
    {
        if (!free_head)
        {
            return nullptr;
        }

        Order *allocated_order = free_head;
        free_head = free_head->next;

        allocated_order->next = nullptr;
        allocated_order->prev = nullptr;

        return allocated_order;
    }

    void OrderPool::deallocate(Order *order)
    {
        order->next = free_head;
        free_head = order;
    }
}