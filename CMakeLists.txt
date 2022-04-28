cmake_minimum_required(VERSION 3.16)
project(antivirus)

set(CMAKE_CXX_STANDARD 20)

add_executable(antivirus src/main.cpp src/data_management.cpp src/crypto_functions.cpp headers/crypto_functions.h headers/data_management.h src/scanner.cpp headers/scanner.h headers/main.h src/quarantine.cpp headers/quarantine.h src/monitor.cpp headers/monitor.h)

target_link_libraries(antivirus ${CMAKE_SOURCE_DIR}/libs/libcrypto++.a)