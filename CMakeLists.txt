cmake_minimum_required(VERSION 3.30)
project(sim)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)

# Library target
add_library(SimFramework)
target_include_directories(SimFramework PUBLIC include)

# Subdirectories
add_subdirectory(src)
add_subdirectory(examples)

# Options
target_compile_options(SimFramework PUBLIC
        -Wall -Wextra -Wpedantic #-Werror
)

# FetchContent
include(FetchContent)

# Raylib
FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 5.5
)
FetchContent_MakeAvailable(raylib)

# Raylib-cpp
FetchContent_Declare(
        raylib-cpp
        GIT_REPOSITORY https://github.com/RobLoach/raylib-cpp
        GIT_TAG v5.5.0
)
FetchContent_MakeAvailable(raylib-cpp)

add_library(raylib-cpp INTERFACE)
target_include_directories(raylib-cpp INTERFACE ${raylib-cpp_SOURCE_DIR}/include)
target_link_libraries(raylib-cpp INTERFACE raylib)
