# Match all .cpp files in the current directory
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

target_sources(SimFramework PRIVATE ${SRC_FILES})
target_link_libraries(SimFramework PRIVATE raylib raylib-cpp)
