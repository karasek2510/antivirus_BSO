//
// Created by karasek on 3/20/22.
//

#ifndef ANTIVIRUS_SCANNER_H
#define ANTIVIRUS_SCANNER_H

#include <string>
#include <unordered_set>

void scan(std::string& path, std::unordered_set<std::string>& hashesSet);

#endif //ANTIVIRUS_SCANNER_H
