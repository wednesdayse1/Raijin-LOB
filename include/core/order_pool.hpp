#pragma

#include "types.hpp"
#include <cstddef>

namespace raijin
{
    class OrderPool
    {
    private:
        Order *arena;     // massive contiguous block we will be using to store orders... reduces latency compared to new
        Order *free_head; // pointer to the next available blank order
        size_t capacity;  // total number of orders we pre allcoated
    public:
        explicit OrderPool(size_t cap); // we call this constructor once at the start of the program for memory allcoation
        ~OrderPool();                   // destructor to clean up the momey when the program is shut down

        // this is asafety measure to prevent copying
        OrderPool(const OrderPool &) = delete;
        OrderPool &operator=(const OrderPool &) = delete;

        Order *allocate();
        void deallocate(Order *order);
    };
}