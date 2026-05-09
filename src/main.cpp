#include "../include/core/limit_order_book.hpp"
#include <iostream>
#include <chrono>

int main()
{
    std::cout << "--- Booting Raijin-LOB ---" << std::endl;
    raijin::LimitOrderBook book(1000000);
    std::cout << "Live. Memory Pool Allocated! Running Latency Tests." << std::endl;
    // synthetic data
    uint64_t start_price = 15000;
    uint32_t standard_volume = 100;
    int num_orders_to_inject = 100000;

    auto start_time = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < num_orders_to_inject; ++i)
    {
        // i is out order id here, and price will be shifted slightly to simulate real market action.
        book.add_order(i, start_price + (i % 10), standard_volume, true);
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();
    auto nanoseconds_per_order = duration / num_orders_to_inject;

    std::cout << "Successfully routed " << num_orders_to_inject << " orders." << std::endl;
    std::cout << "Total time: " << duration / 1000000.0 << " milliseconds." << std::endl;
    std::cout << "Average latency per order: " << nanoseconds_per_order << " nanoseconds." << std::endl;

    return 0;
}