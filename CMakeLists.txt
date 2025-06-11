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

# Doxygen
find_package(Doxygen REQUIRED)

set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

# Doxygen theme
FetchContent_Declare(
        doxygen-awesome-css
        GIT_REPOSITORY https://github.com/jothepro/doxygen-awesome-css.git
        GIT_TAG        v2.3.4  # or another stable tag
)
FetchContent_MakeAvailable(doxygen-awesome-css)
set(DOXYGEN_AWESOME_DIR ${doxygen-awesome-css_SOURCE_DIR})

configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

add_custom_target(doc
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM
)