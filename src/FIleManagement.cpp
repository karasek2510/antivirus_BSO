#include "../headers/FileManagement.h"

#include <sys/vfs.h>

#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_set>

std::array<std::uint64_t, 2> stringHashToUint64s(const std::string &strHash) {
    char delimiter = ',';
    unsigned long index = strHash.find(delimiter);
    std::uint64_t value1 = std::stoull(strHash.substr(0, index));
    std::uint64_t value2 = std::stoull(strHash.substr(index + 1));
    return std::array<std::uint64_t, 2>{value1, value2};
}

bool getFileContent(std::unordered_set<std::array<std::uint64_t, 2>, HashArrayUint64_t> &uSet, const std::string &file) {
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

bool isHashInUnorderedSet(std::unordered_set<std::array<std::uint64_t, 2>, HashArrayUint64_t> &uSet,
                          std::array<std::uint64_t, 2> hash) {
    return uSet.find(hash) != uSet.end();
}


int checkFileFs(const std::filesystem::path &path) {
    struct statfs fileFs{};
    if (statfs(path.c_str(), &fileFs) != 0) {
        return -1;
    }
    return static_cast<int>(fileFs.f_type);
}
