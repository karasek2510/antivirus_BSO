//
// Created by karasek on 3/11/22.
//

#ifndef ANTIVIRUS_BSO_FILEMANAGEMENT_H
#define ANTIVIRUS_BSO_FILEMANAGEMENT_H

std::vector<std::string> listFiles(const std::string& path);
void appendToFile(const std::string& file, std::string str);
void getFileContent(std::unordered_set<std::string>& uSet, std::string file);
bool isStrInUnorderedSet(std::unordered_set<std::string>& uSet, const std::string& str);
void removeExec(const std::string& file);

#endif //ANTIVIRUS_BSO_FILEMANAGEMENT_H