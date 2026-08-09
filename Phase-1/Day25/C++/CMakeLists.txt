cmake_minimum_required(VERSION 3.15)
project(cpp_pager_test CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer")

include_directories(include)

add_executable(cpp_pager_test
    src/BufferPoolManager.cpp
    src/main.cpp
)