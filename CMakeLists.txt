cmake_minimum_required(VERSION 2.8)
project(socketlog)

set(CMAKE_CXX_STANDARD 11)

set(SOCKET_LOG_PATH ./socketlog)
aux_source_directory(${SOCKET_LOG_PATH}/lib SOCKET_LOG_LIB_SRC)
set(SOCKET_LOG_SRC ${SOCKET_LOG_LIB_SRC} ${SOCKET_LOG_PATH}/SocketLog.cpp)

link_libraries(pthread)

add_executable(socketlog
        main.cpp
        ${SOCKET_LOG_LIB_SRC}
        ${SOCKET_LOG_SRC}
        )
