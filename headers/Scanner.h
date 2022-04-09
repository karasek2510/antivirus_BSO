
#ifndef ANTIVIRUS_SCANNER_H
#define ANTIVIRUS_SCANNER_H

#include <string>
#include <unordered_set>

std::unordered_set<std::array<std::uint64_t, 2>, HashArrayUint64_t> hashesSet;

bool scanFile(std::filesystem::path &path);

void scanFiles(std::filesystem::path &path);

#endif //ANTIVIRUS_SCANNER_H
