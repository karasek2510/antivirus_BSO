cmake_minimum_required(VERSION 3.16)
project(antivirus_BSO)

set(CMAKE_CXX_STANDARD 20)

link_libraries(crypto)

add_executable(src/main.cpp src/MD5.cpp headers/MD5.h src/FileManagement.cpp headers/FileManagement.h)
