cmake_minimum_required(VERSION 3.13)
project(TrafficLights)

set(CMAKE_CXX_STANDARD 17)

add_executable(TrafficLights main.cpp Car.cpp Vehicle.cpp)

target_link_libraries(TrafficLights -lncurses -lpthread)