cmake_minimum_required (VERSION 2.6)

project (TcpClient)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -std=c++0x -O0 -fpermissive")
add_executable(TcpClient main.cpp TcpClient.cpp)
TARGET_LINK_LIBRARIES(TcpClient pthread)