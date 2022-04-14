cmake_minimum_required(VERSION 3.16)
project(antivirus)

set(CMAKE_CXX_STANDARD 20)

add_executable(antivirus src/Main.cpp src/DataManagement.cpp src/CryptoFuntions.cpp headers/CryptoFuntions.h headers/DataManagement.h src/Scanner.cpp headers/Scanner.h headers/Main.h src/Quarantine.cpp headers/Quarantine.h)

target_link_libraries(antivirus ${CMAKE_SOURCE_DIR}/libs/libcrypto++.a)