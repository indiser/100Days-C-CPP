cmake_minimum_required(VERSION 4.1)
project(day_3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Static lib
add_library(math_static STATIC src/math_functions.cpp)
target_include_directories(math_static PUBLIC include)

# Shared lib (.dll / .so)
add_library(math_shared SHARED src/math_functions.cpp)
target_include_directories(math_shared PUBLIC include)

# Executable with static lib
add_executable(app_static src/main.cpp)
target_link_libraries(app_static PRIVATE math_static)

# Executable with shared lib
add_executable(app_shared src/main.cpp)
target_link_libraries(app_shared PRIVATE math_shared)