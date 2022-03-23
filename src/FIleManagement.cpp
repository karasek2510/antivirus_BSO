//
// Created by karasek on 3/11/22.
//

#include <vector>
#include <string>
#include <filesystem>
#include <fstream>
#include <unordered_set>
#include <iostream>

#include "../headers/FileManagement.h"


void appendToFile(const std::string &file, const std::string &string) {
    std::ofstream fileOut;
    fileOut.open(file, std::ios_base::app);
    fileOut << string << "\n";
}

bool getFileContent(std::unordered_set<std::string> &uSet, const std::string &file) {
    std::ifstream fileIn(file);
    if (!fileIn) {
        return false;
    }
    std::string line;
    while (std::getline(fileIn, line)) {
        if (!line.empty()) {
            uSet.insert(line);
        }
    }
    fileIn.close();
    return true;
}

bool isStrInUnorderedSet(std::unordered_set<std::string> &uSet, const std::string &str) {
    return uSet.find(str) != uSet.end();
}

void removeExec(const std::string &file) {

    std::filesystem::permissions(file, std::filesystem::perms::owner_exec |
                                       std::filesystem::perms::group_exec |
                                       std::filesystem::perms::others_exec,
                                 std::filesystem::perm_options::remove);
}

void copyFile(const std::string &file, std::string directory) {
    std::string baseFilename = file.substr(file.find_last_of("/\\") + 1);
    std::string tempFileInDirectory = directory.append("/").append(baseFilename);
    int counter = 0;
    std::string fileInDirectory;
    do {
        fileInDirectory = tempFileInDirectory;
        fileInDirectory.append("_").append(std::to_string(counter));
        counter++;
    } while (std::filesystem::exists(fileInDirectory));
    std::filesystem::copy(file, fileInDirectory);
}