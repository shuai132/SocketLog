cmake_minimum_required(VERSION 2.8)

add_compile_options(-std=c++11 -Wall)

link_libraries(socketlog pthread)

add_executable(loopsender loopsender.cpp)

add_executable(benchmark benchmark.cpp)
