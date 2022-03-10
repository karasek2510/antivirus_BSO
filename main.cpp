#include <chrono>
#include <iostream>
#include "MD5.h"

int main() {

    auto start = std::chrono::high_resolution_clock::now();
    std::cout << md5FromFile("qwe.bmp").value() << "\n";
    auto stop = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> ms_double = stop-start;
    std::cout << ms_double.count() << "ms\n";

}