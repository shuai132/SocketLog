cmake_minimum_required(VERSION 2.8)

add_compile_options(-std=c++11 -Wall)

aux_source_directory(./lib SOCKET_LOG_LIB_SRC)

add_library(socketlog ${SOCKET_LOG_LIB_SRC} SocketLog.cpp)
