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
    std::optional<std::filesystem::path> regularFilePath = CheckFileBeforeScanning(path);
    if(!regularFilePath){
        return false;
    }
    std::optional<std::array<std::uint64_t, 2>> md5 = Md5FromFile(regularFilePath.value()); // computing hash
    if (!md5) { // hash couldn't be computed
        std::cout << regularFilePath.value().string() << " -> Cannot evaluate hash" << "\n";
        return false;
    }
    if (!IsHashInUnorderedSet(hashesSet, md5.value())) {
        std::cout << regularFilePath.value().string() << " -> OK" << "\n";
        return true;
    }
    std::cout << regularFilePath.value().string() << " -> MATCHED" << "\n"; // hash was in hash database
    if (!DoQuarantine(regularFilePath.value())) { // moving to quarantine
        std::cerr << "Quarantine was not successfully imposed" << "\n";
        return false;
    }
    filesAddedToQuarantine.push_back(path.string());
    std::cout << "Quarantine was successfully imposed" << "\n";
    return true;

}

void ScanFiles(std::filesystem::path &path, bool (*ScanningFunction)(const std::filesystem::path&)) {
    std::uint64_t numberOfFiles = 0;
    if (!std::filesystem::is_directory(path)) {
        ScanningFunction(path);
    } else {
        try {
            for (const auto &file: std::filesystem::recursive_directory_iterator( // recursively iterating through given directory
                    path, std::filesystem::directory_options::skip_permission_denied)) {
                ScanningFunction(file);
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

