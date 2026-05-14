#include "../../include/core/config.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <stdexcept>

namespace raijin
{
    BookConfig load_config(const std::string &filepath)
    {
        std::ifstream file(filepath);
        if (!file.is_open())
        {
            throw std::runtime_error("Failed to open config file: " + filepath);
        }
        nlohmann::json j;
        file >> j;

        return BookConfig{
            .order_pool_capacity = j.at("order_pool_capacity").get<std::size_t>(),
            .price_level_count = j.at("price_level_count").get<std::uint32_t>(),
            .level_queue_capacity = j.at("level_queue_capacity").get<std::uint32_t>(),
            .max_order_id = j.at("max_order_id").get<std::uint64_t>()};
    }
}