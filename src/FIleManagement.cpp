//
// Created by karasek on 3/11/22.
//

#include <string>
#include <filesystem>
#include <fstream>
#include <unordered_set>
#include <iostream>

#include "../headers/FileManagement.h"



std::array<std::uint64_t,2> stringHashToUint64s(std::string strHash){
    char delimiter = ',';
    int index = strHash.find(delimiter);
    std::uint64_t value1 = std::stol(strHash.substr(0,index));
    std::uint64_t value2 = std::stol(strHash.substr(index+1));
    return std::array<std::uint64_t,2>{value1,value2};
}


void appendToFile(const std::string &file, const std::string &string) {
    std::ofstream fileOut;
    fileOut.open(file, std::ios_base::app);
    fileOut << string << "\n";
}

bool getFileContent(std::unordered_set<std::array<std::uint64_t,2>,HashArrayUint64_t>& uSet, const std::string &file) {
    std::ifstream fileIn(file);
    if (!fileIn) {
        return false;
    }
    std::string line;
    while (std::getline(fileIn, line)) {
        if (!line.empty()) {
            uSet.insert(stringHashToUint64s(line));
        }
    }
    fileIn.close();
    return true;
}

bool isHexInUnorderedSet(std::unordered_set<std::array<std::uint64_t,2>, HashArrayUint64_t>  &uSet, std::array<std::uint64_t,2> hash) {
    return uSet.find(hash) != uSet.end();
}

void removeExec(const std::string &file) {

    std::filesystem::permissions(file, std::filesystem::perms::owner_exec |
                                       std::filesystem::perms::group_exec |
                                       std::filesystem::perms::others_exec,
                                 std::filesystem::perm_options::remove);
}

std::filesystem::path getFullPathQuarantine(const std::filesystem::path &file, const std::filesystem::path &directory) {
    std::string baseFilename = file.stem();
    std::string fileExtension = file.extension();
    int counter = 0;
    std::string fileInDirectory;
    std::string tempFileInDirectory;
    tempFileInDirectory.append(directory).append("/").append(baseFilename).append("_");
    do {
        fileInDirectory = tempFileInDirectory;
        fileInDirectory.append(std::to_string(counter)).append(fileExtension);
        counter++;
    } while (std::filesystem::exists(fileInDirectory));
    return fileInDirectory;
}