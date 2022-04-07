
#include <filesystem>
#include <iostream>
#include <string>

#include "../headers/CryptoFuntions.h"
#include "../headers/FileManagement.h"
#include "../headers/Main.h"
#include "../headers/Quarantine.h"

bool scanFile(const std::filesystem::path path) {
    if (checkFileFs(path) != 61267) {
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
            if (!std::filesystem::is_regular_file(regularFilePath) || checkFileFs(regularFilePath) != 61267) {
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

    std::optional<std::array<std::uint64_t, 2>> md5 = md5FromFile(regularFilePath);
    if (!md5) {
        std::cout << regularFilePath.string() << " -> Cannot evaluate hash" << "\n";
        return false;
    }
    if (!isHexInUnorderedSet(hashesSet, md5.value())) {
        std::cout << regularFilePath.string() << " -> OK" << "\n";
        return false;
    }
    std::cout << regularFilePath.string() << " -> MATCHED" << "\n";
    if (!doQuarantine(regularFilePath)) {
        std::cerr << "Quarantine was not successfully imposed" << "\n";
        return false;
    }
    std::cout << "Quarantine was successfully imposed" << "\n";
    return true;

}

void scanFiles(std::filesystem::path &path) {
    std::uint64_t counter = 0;
    if (!std::filesystem::is_directory(path)) {
        scanFile(path);
    } else {
        try {
            for (const auto &file: std::filesystem::recursive_directory_iterator(path,
                                                                                 std::filesystem::directory_options::skip_permission_denied)) {
                scanFile(file);
                counter++;
            }
        } catch (std::filesystem::filesystem_error const &ex) {
            std::cout << ex.what() << "\n";
        }
        std::cout << "Scanned files: " << counter << "\n";
    }
}
