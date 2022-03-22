//
// Created by karasek on 3/20/22.
//

#include <string>
#include <unordered_set>
#include <filesystem>
#include <iostream>
#include "../headers/MD5.h"
#include "../headers/FileManagement.h"

void scan(std::string& path, std::unordered_set<std::string>& hashesSet){
    if(std::filesystem::is_regular_file(path)){
        std::string md5Hash = md5FromFile(path).value_or("Cant evaluate hash!");
        if(isStrInUnorderedSet(hashesSet,md5Hash)) {
            removeExec(path);
            std::string quarantineDir = getenv("HOME");
            quarantineDir += "/.danger";
            std::filesystem::create_directory(quarantineDir);
            try {
                copyFile(path,quarantineDir);
//                    std::filesystem::remove(file);
            } catch (std::filesystem::filesystem_error& e) {
                std::cerr << e.what() << '\n';
            }
        }
    }else if(std::filesystem::is_symlink(path)){

    }else{

    }
}
