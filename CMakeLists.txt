cmake_minimum_required(VERSION 3.10)

project(RaijinLOB CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# -Wall -Wextra : Turns on all warnings.
# -Werror : Treats every warning as a hard crash. 
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")

#Force Hardware Level 3 Optimization (-O3)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -DNDEBUG")


include_directories(include)

add_executable(raijin_engine 
    src/core/order_pool.cpp 
    src/core/limit_order_book.cpp 
    src/main.cpp
)