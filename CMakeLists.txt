cmake_minimum_required(VERSION 3.29)
project(TraceBezierCurve)

set(CMAKE_CXX_STANDARD 20)

add_executable(TraceBezierCurve main.cpp bezier.h)
add_executable(benchmark benchmark.cpp)
