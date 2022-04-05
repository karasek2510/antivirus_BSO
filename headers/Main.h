//
// Created by karasek on 3/22/22.
//

#ifndef ANTIVIRUS_MAIN_H
#define ANTIVIRUS_MAIN_H

#include <iostream>
#include <string>
#include <unordered_set>
#include <filesystem>

extern std::unordered_set<std::array<std::uint64_t,2>, HashArrayUint64_t> hashesSet;
extern std::filesystem::path quarantineDirectory;


#endif //ANTIVIRUS_MAIN_H
