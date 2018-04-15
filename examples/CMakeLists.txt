cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_STANDARD 11)

link_libraries(socketlog pthread)

add_executable(loopsender loopsender.cpp)

add_executable(benchmark benchmark.cpp)
