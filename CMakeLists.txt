cmake_minimum_required(VERSION 3.21)
project(antivirus)

set(CMAKE_CXX_STANDARD 20)

link_libraries(crypto)
link_libraries(crypto++)

add_executable(antivirus src/Main.cpp src/FIleManagement.cpp src/CryptoFuntions.cpp headers/CryptoFuntions.h headers/FileManagement.h src/Scanner.cpp headers/Scanner.h headers/Main.h src/Quarantine.cpp headers/Quarantine.h)
