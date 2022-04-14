
#ifndef ANTIVIRUS_SCANNER_H
#define ANTIVIRUS_SCANNER_H

#include <filesystem>
#include <string>
#include <unordered_set>
#include "DataManagement.h"


bool IsHashInUnorderedSet(std::unordered_set<std::array<std::uint64_t, 2>, HashArrayUint64_t> &uSet,
                          std::array<std::uint64_t, 2> hash);

bool ScanFile(const std::filesystem::path& path);

void ScanFiles(std::filesystem::path &path);

bool GetHashesFromFile(std::unordered_set<std::array<std::uint64_t, 2>, HashArrayUint64_t> &uSet, const std::string &file);




#endif //ANTIVIRUS_SCANNER_H
