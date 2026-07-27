cmake_minimum_required(VERSION 4.1)
project(day_03)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# static
add_library(math_functions_static STATIC src/math_functions.c)
target_include_directories(math_functions_static PUBLIC include)

# Shared
add_library(math_functions_shared SHARED src/math_functions.c)
target_include_directories(math_functions_shared PUBLIC include)

# Executable Linking Static
add_executable(app_static src/main.c)
target_link_libraries(app_static PRIVATE math_functions_static)

# Executable linking Shared
add_executable(app_shared src/main.c)
target_link_libraries(app_shared PRIVATE math_functions_shared)