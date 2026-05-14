#include "../include/core/limit_order_book.hpp"
#include "../include/core/config.hpp"
#include <iostream>

using namespace raijin;

int main()
{
    std::cout << "--- Booting Raijin LOB ---" << std::endl;
    try
    {
        BookConfig config = load_config("config/settings.json");
        LimitOrderBook book(config);
        std::cout << "Config Loaded, Mempool Allocated!" << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Fatal Startup Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
