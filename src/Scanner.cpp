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

bool scanFile(std::filesystem::path path) {
    if(path=="/proc/kcore" ||
            path=="/proc/net"
            ){
        return false;
    }
    std::filesystem::path regularFilePath;
    if(std::filesystem::is_symlink(path)) {
        std::cout << "Symlink: " << path << "\n";                            // LOGGER
        try {
            regularFilePath = std::filesystem::canonical(
                    path.parent_path().append(std::filesystem::read_symlink(path).string()));
        } catch(const std::exception& ex){
            std::cout << ex.what() << "\n";
            return false;
        }
        if (!std::filesystem::is_regular_file(regularFilePath)) {
            return false;
        }
    } else if (std::filesystem::is_regular_file(path)){
        regularFilePath = path;
    }
    if(!(regularFilePath.empty() || std::filesystem::is_empty(regularFilePath))){
        std::cout << regularFilePath << "\n";                                           // LOGGER
        std::optional<std::string> md5 = md5FromFile(regularFilePath);
        if (md5 && isStrInUnorderedSet(hashesSet, md5.value())) {
            std::cout << regularFilePath << " matched!\n";                                         // LOGGER
            doQuarantine(regularFilePath);
            return true;
        }
    }
    return false;
}

void scanFiles(std::string &path) {
    if (!std::filesystem::is_directory(path)) {
        scanFile(path);
    } else {
        for (const auto &file: std::filesystem::recursive_directory_iterator(path,std::filesystem::directory_options::skip_permission_denied)) {
            scanFile(file);
        }
    }
}
