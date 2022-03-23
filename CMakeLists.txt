cmake_minimum_required(VERSION 3.21)
project(antivirus)

set(CMAKE_CXX_STANDARD 20)

link_libraries(crypto)

add_executable(antivirus src/Main.cpp src/FIleManagement.cpp src/MD5.cpp headers/MD5.h headers/FileManagement.h src/Scanner.cpp headers/Scanner.h headers/Main.h)
