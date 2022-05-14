cmake_minimum_required(VERSION 3.16)
project(antivirus)

set(CMAKE_CXX_STANDARD 20)

find_package(Threads REQUIRED)

file(GLOB_RECURSE antivirus_SRC
        "headers/*.h"
        "src/*.cpp"
        "yaracpp/*.cpp"
        "yaracpp/*.h"
        )

add_executable(antivirus ${antivirus_SRC})

#add_executable(antivirus src/main.cpp src/data_management.cpp src/crypto_functions.cpp headers/crypto_functions.h headers/data_management.h src/scanner.cpp headers/scanner.h headers/main.h src/quarantine.cpp headers/quarantine.h src/monitor.cpp headers/monitor.h headers/thread_safe_queue.h headers/yaraHead.h src/yara.cpp yaracpp/src/yara_detector/yara_detector.cpp)

target_link_libraries(antivirus ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(antivirus ${CMAKE_SOURCE_DIR}/libs/libcrypto++.a)
target_link_libraries(antivirus ${CMAKE_SOURCE_DIR}/libs/libyara.so)