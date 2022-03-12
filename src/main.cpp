#include <chrono>
#include <iostream>
#include <unordered_set>
#include "../headers/MD5.h"
#include "../headers/FileManagement.h"

int main(){
    auto start = std::chrono::high_resolution_clock::now();
    std::unordered_set<std::string> hashes;
    getFileContent(hashes,"resources/hashes.txt");
    std::vector<std::string> files = listFiles("resources");
    std::string md5Hash;
    for(std::string file : files) {
        md5Hash = md5FromFile(file).value_or(file);
        std::cout << file << " : " << md5Hash << "\n";
        appendToFile("resources/hashes.txt", md5Hash);
//        if(isStrInUnorderedSet(hashes,md5Hash)){
//            std::cout <<"Found -> "<< file <<": "<< md5Hash << "\n";
//        }

    }
    auto stop = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> ms_double = stop-start;
    std::cout << ms_double.count() << "ms\n";

}