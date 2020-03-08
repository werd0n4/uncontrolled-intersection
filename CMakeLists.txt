cmake_minimum_required(VERSION 3.16)
project(TrafficLights)

set(CMAKE_CXX_STANDARD 14)

add_executable(TrafficLights main.cpp)

target_link_libraries(TrafficLights -lncurses -lpthread)