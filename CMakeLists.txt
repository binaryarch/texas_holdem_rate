cmake_minimum_required(VERSION 3.5)
project(texas_holdem_rate)

include_directories(src)

set(CMAKE_CXX_FLAGS "-std=c++20")

add_executable(texas_holdem_rate  main.cpp)