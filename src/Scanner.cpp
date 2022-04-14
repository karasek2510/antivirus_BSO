#include "../headers/Scanner.h"

#include <filesystem>
#include <iostream>
#include <string>

#include "../headers/CryptoFuntions.h"
#include "../headers/Quarantine.h"

std::vector<std::string> filesAddedToQuarantine;

std::unordered_set<std::array<std::uint64_t, 2>, HashArrayUint64_t> hashesSet;

bool IsHashInUnorderedSet(std::unordered_set<std::array<std::uint64_t, 2>, HashArrayUint64_t> &uSet,
                          std::array<std::uint64_t, 2> hash) {
    return uSet.find(hash) != uSet.end();
}

bool ScanFile(const std::filesystem::path& path) {
    if (CheckFileFs(path) != 61267) {
        return false;
    }
    std::filesystem::path regularFilePath = path;
    if (std::filesystem::is_regular_file(path)) {
        if (std::filesystem::is_symlink(path)) {
            try {
                regularFilePath = std::filesystem::canonical(
                        path.parent_path().append(std::filesystem::read_symlink(path).string()));
            } catch (const std::exception &ex) {
                std::cout << path.string() << " -> Cannot read symlink" << "\n";
                return false;
            }
            if (!std::filesystem::is_regular_file(regularFilePath)) {
                std::cout << regularFilePath.string() << " -> Cannot read file" << "\n";
                return false;
            }
        }
    } else {
        return false;
    }
    if (std::filesystem::is_empty(regularFilePath)) {
        std::cout << regularFilePath.string() << " -> Empty file" << "\n";
        return false;
    }
    std::optional<std::array<std::uint64_t, 2>> md5 = Md5FromFile(regularFilePath);
    if (!md5) {
        std::cout << regularFilePath.string() << " -> Cannot evaluate hash" << "\n";
        return false;
    }
    if (!IsHashInUnorderedSet(hashesSet, md5.value())) {
        std::cout << regularFilePath.string() << " -> OK" << "\n";
        return false;
    }
    std::cout << regularFilePath.string() << " -> MATCHED" << "\n";
    if (!DoQuarantine(regularFilePath)) {
        std::cerr << "Quarantine was not successfully imposed" << "\n";
        return false;
    }
    filesAddedToQuarantine.push_back(path.string());
    std::cout << "Quarantine was successfully imposed" << "\n";
    return true;

}

void ScanFiles(std::filesystem::path &path) {
    std::uint64_t counter = 0;
    if (!std::filesystem::is_directory(path)) {
        ScanFile(path);
    } else {
        try {
            for (const auto &file: std::filesystem::recursive_directory_iterator(path,
                                                                                 std::filesystem::directory_options::skip_permission_denied)) {
                ScanFile(file);
                counter++;
            }
        } catch (std::filesystem::filesystem_error const &ex) {
            std::cout << ex.what() << "\n";
        }
        std::cout << "Scanned files: " << counter << "\n";
        if(filesAddedToQuarantine.empty()){
            std::cout << "No files have been put in quarantine!\n";
        }else{
            std::cout << "Files which have been put in quarantine: \n";
            for(std::string s : filesAddedToQuarantine){
                std::cout << "\t- " << s << "\n";
            }
            std::cout << "For detailed information use subcommand \"showQuarantine\".\n";
        }
    }
}

bool GetHashesFromFile(std::unordered_set<std::array<std::uint64_t, 2>, HashArrayUint64_t> &uSet, const std::string &file) {
    std::ifstream fileIn(file);
    if (!fileIn) {
        return false;
    }
    std::string line;
    while (std::getline(fileIn, line)) {
        if (!line.empty()) {
            uSet.insert(StringHashToUint64s(line));
        }
    }
    fileIn.close();
    return true;
}

