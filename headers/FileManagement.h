//
// Created by karasek on 3/11/22.
//

#ifndef ANTIVIRUS_BSO_FILEMANAGEMENT_H
#define ANTIVIRUS_BSO_FILEMANAGEMENT_H

#include <unordered_set>
#include <filesystem>

class HashArrayUint64_t {
public:
    size_t operator()(const std::array<std::uint64_t,2>& arr) const{
        size_t result = 0;
        for(std::uint64_t val : arr){
            val ^= val >> 33;
            val *= 0xff51afd7ed558ccd;
            val ^= val >> 33;
            val *= 0xc4ceb9fe1a85ec53;
            val ^= val >> 33;
            result ^= val;
        }
        return result;
    }
};

std::array<std::uint64_t,2> stringHashToUint64s(std::string strHash);

void appendToFile(const std::string &file, const std::string &str);

bool getFileContent(std::unordered_set<std::array<std::uint64_t,2>,HashArrayUint64_t>& uSet, const std::string &file);

bool isHexInUnorderedSet(std::unordered_set<std::array<std::uint64_t,2>, HashArrayUint64_t>  &uSet, const std::array<std::uint64_t,2>);

void removeExec(const std::string &file);

std::filesystem::path getFullPathQuarantine(const std::filesystem::path &file, const std::filesystem::path &directory);

#endif //ANTIVIRUS_BSO_FILEMANAGEMENT_H