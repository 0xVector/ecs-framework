cmake_minimum_required(VERSION 3.30)
project(sim)

set(CMAKE_CXX_STANDARD 23)

# Library target
add_library(SimFramework INTERFACE)
target_include_directories(SimFramework INTERFACE include/)

add_subdirectory(src)
