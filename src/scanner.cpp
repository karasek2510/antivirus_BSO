#include "../headers/scanner.h"

#include <filesystem>
#include <iostream>
#include <string>
#include <thread>

#include "../headers/crypto_functions.h"
#include "../headers/quarantine.h"

// global variable containing list of files which were added to quarantine in scan
std::vector<std::string> filesAddedToQuarantine;

// global variable from Main.h
std::unordered_set<std::array<std::uint64_t, 2>, HashArrayUint64_t> hashesSet;

// check if hash is in given unordered_set
bool IsHashInUnorderedSet(std::unordered_set<std::array<std::uint64_t, 2>, HashArrayUint64_t> &uSet,
                          std::array<std::uint64_t, 2> hash) {
    return uSet.find(hash) != uSet.end();
}


bool ScanFile(const std::filesystem::path &path) {
//    std::cout << "Curr thread: " << std::this_thread::get_id << "\n";
    if (CheckFileFs(path) != 61267) { // filtering files with specific filesystem
        return false;
    }
    std::filesystem::path regularFilePath = path;
    if (std::filesystem::is_regular_file(path)) {
        if (std::filesystem::is_symlink(path)) {
            try {
                regularFilePath = std::filesystem::canonical(path.parent_path().append(  // following symlink
                        std::filesystem::read_symlink(path).string()));
            } catch (const std::exception &ex) {
                std::cout << path.string() << " -> Cannot read symlink" << "\n";
                return false;
            }
            if (!std::filesystem::is_regular_file(regularFilePath)) { // if symlink is pointing on not regular file
                std::cout << regularFilePath.string() << " -> Cannot read file" << "\n";
                return false;
            }
        }
    } else {
        return false;
    }
    if (std::filesystem::is_empty(regularFilePath)) { // file is empty
        std::cout << regularFilePath.string() << " -> Empty file" << "\n";
        return false;
    }
    std::optional<std::array<std::uint64_t, 2>> md5 = Md5FromFile(regularFilePath); // computing hash
    if (!md5) { // hash couldn't be computed
        std::cout << regularFilePath.string() << " -> Cannot evaluate hash" << "\n";
        return false;
    }
    if (!IsHashInUnorderedSet(hashesSet, md5.value())) {
        std::cout << regularFilePath.string() << " -> OK" << "\n";
        return false;
    }
    std::cout << regularFilePath.string() << " -> MATCHED" << "\n"; // hash was in hash database
    if (!DoQuarantine(regularFilePath)) { // moving to quarantine
        std::cerr << "Quarantine was not successfully imposed" << "\n";
        return false;
    }
    filesAddedToQuarantine.push_back(path.string());
    std::cout << "Quarantine was successfully imposed" << "\n";
    return true;

}

void ScanFiles(std::filesystem::path &path) {
    std::uint64_t numberOfFiles = 0;
    if (!std::filesystem::is_directory(path)) {
        ScanFile(path);
    } else {
        try {
            for (const auto &file: std::filesystem::recursive_directory_iterator( // recursively iterating through given directory
                    path, std::filesystem::directory_options::skip_permission_denied)) {
                ScanFile(file);
                numberOfFiles++;
            }
        } catch (std::filesystem::filesystem_error const &ex) { // possible iterator exception
            std::cerr << "The files cannot be iterated any further" << "\n";
        }
        std::cout << "Scanned files: " << numberOfFiles << "\n";
        if (filesAddedToQuarantine.empty()) {
            std::cout << "No files have been put in quarantine!\n";
        } else {
            std::cout << "Files which have been put in quarantine: \n";
            for (const std::string &s: filesAddedToQuarantine) { // summary about files added to quarantine
                std::cout << "\t- " << s << "\n";
            }
            std::cout << "For detailed information use subcommand \"showQuarantine\".\n";
        }
    }
}

// Loading hashes from file
bool GetHashesFromFile(std::unordered_set<std::array<std::uint64_t, 2>, HashArrayUint64_t> &uSet, const std::string &file) {
    std::ifstream fileIn(file);
    if (!fileIn) {
        return false;
    }
    std::string line;
    try{
        while (std::getline(fileIn, line)) {
            if (!line.empty()) {
                uSet.insert(StringHashToUint64s(line));
            }
        }
    }catch (std::invalid_argument &ex){
        return false;
    }
    fileIn.close();
    return true;
}

