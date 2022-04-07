
#include <sys/vfs.h>

#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_set>

#include "../headers/FileManagement.h"

std::array<std::uint64_t, 2> stringHashToUint64s(const std::string &strHash) {
    char delimiter = ',';
    unsigned long index = strHash.find(delimiter);
    std::uint64_t value1 = std::stol(strHash.substr(0, index));
    std::uint64_t value2 = std::stol(strHash.substr(index + 1));
    return std::array<std::uint64_t, 2>{value1, value2};
}

bool
getFileContent(std::unordered_set<std::array<std::uint64_t, 2>, HashArrayUint64_t> &uSet, const std::string &file) {
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

bool isHexInUnorderedSet(std::unordered_set<std::array<std::uint64_t, 2>, HashArrayUint64_t> &uSet,
                         std::array<std::uint64_t, 2> hash) {
    return uSet.find(hash) != uSet.end();
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

int checkFileFs(const std::filesystem::path &path) {
    struct statfs fileFs;
    if (statfs(path.c_str(), &fileFs) != 0) {
        return -1;
    }
    return fileFs.f_type;
}
