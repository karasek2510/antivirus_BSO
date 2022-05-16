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

target_link_libraries(antivirus ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(antivirus ${CMAKE_SOURCE_DIR}/libs/libcrypto++.a)
target_link_libraries(antivirus ${CMAKE_SOURCE_DIR}/libs/libyara.so)