#pragma once

#include "limit_order_book.hpp"
#include <string>

namespace raijin
{
    BookConfig load_config(const std::string &filepath);
}