cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_STANDARD 11)

aux_source_directory(./lib SOCKET_LOG_LIB_SRC)

add_library(socketlog ${SOCKET_LOG_LIB_SRC} SocketLog.cpp)
