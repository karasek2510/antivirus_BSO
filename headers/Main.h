
#ifndef ANTIVIRUS_MAIN_H
#define ANTIVIRUS_MAIN_H

#include <filesystem>
#include <iostream>
#include <string>
#include <unordered_set>
#include "DataManagement.h"

extern std::unordered_set<std::array<std::uint64_t, 2>, HashArrayUint64_t> hashesSet;

#endif //ANTIVIRUS_MAIN_H
