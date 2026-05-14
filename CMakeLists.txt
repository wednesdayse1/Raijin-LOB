cmake_minimum_required(VERSION 3.14)
project(Raijin-LOB CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -DNDEBUG")

include_directories(include
third_party/json/include)

add_library(core_objects OBJECT
    src/core/order_pool.cpp
    src/core/limit_order_book.cpp
    src/core/config.cpp)

add_executable(raijin_engine src/main.cpp)
target_link_libraries(raijin_engine PRIVATE core_objects)

add_subdirectory(third_party/googletest)

add_executable(test_order_pool tests/test_order_pool.cpp)
target_link_libraries(test_order_pool PRIVATE core_objects GTest::gtest_main)

include(CTest)
add_test(NAME OrderPoolTests COMMAND test_order_pool)
