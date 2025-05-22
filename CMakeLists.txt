cmake_minimum_required(VERSION 3.30)
project(sim)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Library target
add_library(SimFramework INTERFACE)
target_include_directories(SimFramework INTERFACE include)

add_subdirectory(src)
add_subdirectory(examples)

target_compile_options(SimFramework INTERFACE
        -Wall -Wextra -Wpedantic -Werror
)

# Raylib
include(FetchContent)

FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 5.5
)

FetchContent_MakeAvailable(raylib)