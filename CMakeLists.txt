cmake_minimum_required(VERSION 3.30)
project(sim)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Wpedantic -Werror)

# Library target
add_library(SimFramework INTERFACE)
target_include_directories(SimFramework INTERFACE include)

add_subdirectory(src)
add_subdirectory(examples)
