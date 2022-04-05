#include <iostream>
#include <unordered_set>
#include "../headers/FileManagement.h"
#include "../headers/Main.h"
#include <filesystem>
#include "../headers/Scanner.h"
#include "../headers/Quarantine.h"

#include "../headers/CLI11.hpp"


std::filesystem::path quarantineDirectory;

int main(int argc, char **argv) {



    CLI::App app{"Antivirus project by Szymon Kasperek"};

    auto restore = app.add_subcommand("restore", "Restore from quarantine");

    std::string restoreFilename;
    restore->add_option("file", restoreFilename, "File to be restored")->required();

    auto scan = app.add_subcommand("scan", "Scan targets");
    std::filesystem::path target;
    scan->add_option("target", target, "Target to be scanned (file/directory)")
            ->check(CLI::ExistingDirectory | CLI::ExistingFile)
            ->required();

    std::filesystem::path hashes;
    scan->add_option("hashes", hashes, "File containing hashes")
            ->check(CLI::ExistingFile)
            ->required();

    CLI11_PARSE(app, argc, argv)
    if (!(*restore || *scan)) {
        std::cout << "Parameters are required" << "\n";
        std::cout << "Run with --help for more information." << "\n";
        return EXIT_FAILURE;
    }
    std::string homedir = getenv("HOME");
    quarantineDirectory = homedir + "/Q";
    std::cout << "Quarantine directory: " << quarantineDirectory.string() << "\n";
    if (!std::filesystem::exists(quarantineDirectory)) {
        std::cout << "Creating quarantine directory..." << "\n";
        if (!std::filesystem::create_directory(quarantineDirectory)) {
            return EXIT_FAILURE;
        }
    }
    if (*scan) {
        std::cout << "Loading hashes..." << "\n";
        if (!getFileContent(hashesSet, hashes)) {
            std::cerr << "Unable to open file containing hashes" << "\n";
            return EXIT_FAILURE;
        }

        std::cout << "Starting scanning:" << "\n";
        scanFiles(target);
        std::cout << "END!!! \n";
        return EXIT_SUCCESS;
    }
    if (*restore) {
        if (!restoreFromQuarantine(restoreFilename)) {
            std::cerr << "File from quarantine could not be restored" << "\n";
            return EXIT_FAILURE;
        }
        std::cout << "File from quarantine was restored" << "\n";
        return EXIT_SUCCESS;
    }


}