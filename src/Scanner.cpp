//
// Created by karasek on 3/20/22.
//

#include <string>
#include <filesystem>
#include <iostream>
#include "../headers/MD5.h"
#include "../headers/FileManagement.h"


std::unordered_set<std::string> hashesSet;

bool scanFile(std::filesystem::path path) {
    if (std::filesystem::is_symlink(path)) {
        std::string tempPath = path;
        std::cout << "Symlink: " << tempPath << "\n";
        tempPath = std::filesystem::canonical(
                path.parent_path().append(std::filesystem::read_symlink(tempPath).string()));
        if (std::filesystem::is_regular_file(tempPath)) {
            std::optional<std::string> md5 = md5FromFile(tempPath);
            if (md5 && isStrInUnorderedSet(hashesSet, md5.value())) {
                std::cout << tempPath << " matched!\n";
                return true;
            } else {
                return false;
            }
        } else {
            return false;
        }

    } else if (std::filesystem::is_regular_file(path)) {
        std::cout << path << "\n";
        std::optional<std::string> md5 = md5FromFile(path);
        if (md5 && isStrInUnorderedSet(hashesSet, md5.value())) {
            std::cout << path << " matched!\n";
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

void scanFiles(std::string &path) {
    if (!std::filesystem::is_directory(path)) {
        scanFile(path);
    } else {
        for (const auto &file: std::filesystem::recursive_directory_iterator(path)) {
            scanFile(file);
        }
    }
}
