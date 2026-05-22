cmake_minimum_required(VERSION 3.14)

project(RaijinLOB CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -DNDEBUG")

include_directories(
    include
    third_party/json/include
)

add_library(core_objects OBJECT 
    src/core/config.cpp
    src/core/order_pool.cpp 
    src/core/limit_order_book.cpp
)

add_executable(raijin_engine src/main.cpp)
target_link_libraries(raijin_engine PRIVATE core_objects)

add_subdirectory(third_party/googletest)

include(CTest)

file(GLOB TEST_SOURCES CONFIGURE_DEPENDS "tests/*.cpp")
add_executable(raijin_tests ${TEST_SOURCES})
target_link_libraries(raijin_tests PRIVATE core_objects GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(raijin_tests)

set(BENCHMARK_ENABLE_RTTI ON CACHE BOOL "Enable RTTI in Google Benchmark" FORCE)
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Disable benchmark tests" FORCE)

add_subdirectory(third_party/benchmark)

file(GLOB BENCHMARK_SOURCES CONFIGURE_DEPENDS "benchmarks/*.cpp")

add_executable(raijin_benchmarks ${BENCHMARK_SOURCES})
target_link_libraries(raijin_benchmarks PRIVATE core_objects benchmark::benchmark)