cmake_minimum_required(VERSION 3.17)
project(assign4_client)

set(CMAKE_CXX_STANDARD 20)

add_executable(assign4_client main.cpp SimpleTCPSocket.hpp SimpleTCPSocket.cpp)