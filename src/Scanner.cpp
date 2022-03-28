//
// Created by karasek on 3/20/22.
//

#include <string>
#include <filesystem>
#include <iostream>
#include "../headers/CryptoFuntions.h"
#include "../headers/FileManagement.h"
#include "../headers/Quarantine.h"


std::unordered_set<std::string> hashesSet;

bool scanFile(const std::filesystem::path& path) {
    std::filesystem::path regularFilePath;
    if (std::filesystem::is_symlink(path)) {
        std::cout << path.string() << " -> ";
        try {
            regularFilePath = std::filesystem::canonical(
                    path.parent_path().append(std::filesystem::read_symlink(path).string()));
        } catch (const std::exception &ex) {
            std::cout << "Cannot read symlink" << "\n";
            return false;
        }
        if (!std::filesystem::is_regular_file(regularFilePath)) {
            std::cout << "Cannot read file" << "\n";
            return false;
        }
    } else if (std::filesystem::is_regular_file(path)) {
        std::cout << path.string() << " -> ";
        regularFilePath = path;
    }
    if (regularFilePath.empty()) {
        return false;
    }
    if (std::filesystem::is_empty(regularFilePath)) {
        std::cout << "Empty file" << "\n";
        return false;
    }

    std::optional<std::string> md5 = md5FromFile(regularFilePath);
    if (!md5) {
        std::cout << "Cannot evaluate hash" << "\n";
        return false;
    }
    if (!isStrInUnorderedSet(hashesSet, md5.value())) {
        std::cout << "OK" << "\n";
        return false;
    }
    std::cout << "MATCHED" << "\n";
    if (!doQuarantine(regularFilePath)) {
        std::cerr << "Quarantine was not successfully imposed" << "\n";
        return false;
    }
    std::cout << "Quarantine was successfully imposed" << "\n";
    return true;

}

void scanFiles(std::filesystem::path &path) {
    if (!std::filesystem::is_directory(path)) {
        scanFile(path);
    } else {
        for (const auto &file: std::filesystem::recursive_directory_iterator(path,
                                                                             std::filesystem::directory_options::skip_permission_denied)) {
            scanFile(file);
        }
    }
}
