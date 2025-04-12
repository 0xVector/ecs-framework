cmake_minimum_required(VERSION 3.30)
project(sim)

set(CMAKE_CXX_STANDARD 23)

add_executable(sim src/main.cpp
        src/sim/Simulation.h
        src/sim/Entity.h
        src/sim/Event.h)
