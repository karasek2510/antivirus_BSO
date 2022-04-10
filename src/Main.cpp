
#include <filesystem>
#include <iostream>
#include <unordered_set>
#include <csignal>

#include "../libs/CLI11.hpp"
#include "../headers/CryptoFuntions.h"
#include "../headers/FileManagement.h"
#include "../headers/Scanner.h"
#include "../headers/Quarantine.h"

std::filesystem::path quarantineDirectory;

void INThandler(int sig){
    char  c;
    signal(sig, SIG_IGN);
    printf("\nDo you really want to quit? [y/n] ");
    c = getchar();
    if (c == 'y' || c == 'Y')
        throw std::runtime_error("EXITING");
    else
        signal(SIGINT, INThandler);
    getchar();
}

int main(int argc, char **argv) {

    signal(SIGINT, INThandler);
    try{
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

        auto hash = app.add_subcommand("hash", "Calculate hash of the file");
        std::filesystem::path fileToHash;
        hash->add_option("file", fileToHash, "File to be hashed")
                ->check(CLI::ExistingFile)
                ->required();
        auto showQuarantine = app.add_subcommand("showQuarantine", "Show files that are in quarantine");


        CLI11_PARSE(app, argc, argv)
        if (!(*restore || *scan || *hash || *showQuarantine)) {
            std::cout << "Parameters are required" << "\n";
            std::cout << "Run with --help for more information." << "\n";
            return EXIT_FAILURE;
        }

        if (*hash) {
            std::cout << "Computing hash of " << fileToHash.string() << "\n";
            std::optional<std::array<std::uint64_t, 2>> hashFromFile = md5FromFile(fileToHash);
            if (!hashFromFile) {
                std::cerr << "Hash could not be computed" << "\n";
                return EXIT_FAILURE;
            }
            std::cout << "Result: " << hashFromFile.value()[0] << "," << hashFromFile.value()[1] << "\n";
            return EXIT_SUCCESS;
        }

        std::string homedir = getenv("HOME");
        quarantineDirectory = homedir + "/Q";
        std::cout << "Quarantine directory: " << quarantineDirectory.string() << "\n";


        if (!std::filesystem::exists(quarantineDirectory)) {
            std::cout << "Creating quarantine directory..." << "\n";
            if (!std::filesystem::create_directory(quarantineDirectory)) {
                alterQuarantinePermissions(0);
                return EXIT_FAILURE;
            }
        }

        if(!alterQuarantinePermissions(448)){
            return EXIT_FAILURE;
        }

        if(*showQuarantine){
            showFilesInQuarantine();
            alterQuarantinePermissions(0);
            return EXIT_SUCCESS;
        }

        if (*scan) {
            std::cout << "Loading hashes..." << "\n";
            if (!getFileContent(hashesSet, hashes)) {
                std::cerr << "Unable to open file containing hashes" << "\n";
                alterQuarantinePermissions(0);
                return EXIT_FAILURE;
            }

            std::cout << "Starting scanning:" << "\n";
            scanFiles(target);
            std::cout << "Scan has been finished! \n";
            alterQuarantinePermissions(0);
            return EXIT_SUCCESS;
        }
        if (*restore) {
            if (!restoreFromQuarantine(restoreFilename)) {
                std::cerr << "File from quarantine could not be restored" << "\n";
                alterQuarantinePermissions(0);
                return EXIT_FAILURE;
            }
            std::cout << "File from quarantine was restored" << "\n";
            alterQuarantinePermissions(0);
            return EXIT_SUCCESS;
        }
    }catch (std::runtime_error ex){
        alterQuarantinePermissions(0);
        return EXIT_FAILURE;
    }
}