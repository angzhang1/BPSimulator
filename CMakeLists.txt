cmake_minimum_required(VERSION 3.1)

set (CMAKE_CXX_STANDARD 11)

add_executable(simulator src/main.cpp src/branch_predictor.cpp)