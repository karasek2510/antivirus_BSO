//
// Created by karasek on 3/11/22.
//

#ifndef ANTIVIRUS_BSO_FILEMANAGEMENT_H
#define ANTIVIRUS_BSO_FILEMANAGEMENT_H

#include <unordered_set>

void appendToFile(const std::string &file, const std::string &str);

bool getFileContent(std::unordered_set<std::string> &uSet, const std::string &file);

bool isStrInUnorderedSet(std::unordered_set<std::string> &uSet, const std::string &str);

void removeExec(const std::string &file);

std::filesystem::path getFullPathQuarantine(const std::filesystem::path &file, const std::filesystem::path& directory);

#endif //ANTIVIRUS_BSO_FILEMANAGEMENT_H