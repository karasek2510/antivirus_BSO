//
// Created by karasek on 3/20/22.
//

#ifndef ANTIVIRUS_SCANNER_H
#define ANTIVIRUS_SCANNER_H

#include <string>
#include <unordered_set>

bool scanFile(std::filesystem::path path);

void scanFiles(std::string &path);

#endif //ANTIVIRUS_SCANNER_H
