cmake_minimum_required(VERSION 3.21)
project(antivirus_BSO)

set(CMAKE_CXX_STANDARD 20)

link_libraries(crypto)

add_executable(ant main.cpp MD5.cpp MD5.h)
