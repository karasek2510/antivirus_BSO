#include "../headers/main.h"

#include <csignal>
#include <filesystem>
#include <iostream>
#include <unordered_set>

#include "../libs/CLI11.hpp"

#include "../headers/crypto_functions.h"
#include "../headers/scanner.h"
#include "../headers/quarantine.h"
#include "../headers/monitor.h"
#include "../headers/yara.h"


// function handling ctrl+c signal
void CtrlCHandler(int sig) {
    char c;
    signal(sig, SIG_IGN);
    printf("\nDo you really want to quit? [y/n] ");
    c = getchar();
    if (c == 'y' || c == 'Y') {
        AlterQuarantinePermissions(0);
        exit(0);
    } else
        signal(SIGINT, CtrlCHandler);
    getchar();
}

int main(int argc, char **argv) {

    signal(SIGINT, CtrlCHandler);
    CLI::App app{"Antivirus project by Szymon Kasperek"};

    auto restore = app.add_subcommand("restore", "Restore from quarantine");

    std::string restoreFilename;
    restore->add_option("file", restoreFilename,
                        "Filename in quarantine (can be checked in \"showQuarantine\" subcommand) to be restored")
            ->required();

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


    auto monitor = app.add_subcommand("monitor", "Monitoring directory recursively");
    std::string directoryToMonitor;
    monitor->add_option("directory", directoryToMonitor, "Directory to be monitored")
            ->check(CLI::ExistingDirectory)
            ->required();
    monitor->add_option("hashes", hashes, "File containing hashes")
            ->check(CLI::ExistingFile)
            ->required();
    monitor->add_option("max_thread", MAX_THREAD_N, "Max thread number")
            ->required();

    std::vector<std::filesystem::path> rule_paths;
    auto yara = app.add_subcommand("yara", "Scanning with yara");
    yara->add_option("--rules", rule_paths, "Yara rules using in scanning")
            ->check(CLI::ExistingFile)
            ->required();
    yara->add_option("target", target, "Target to be scanned (file/directory)")
            ->check(CLI::ExistingDirectory | CLI::ExistingFile)
            ->required();

    CLI11_PARSE(app, argc, argv)

    // if no subcommand was typed
    if (!(*restore || *scan || *hash || *showQuarantine || *monitor || *yara)) {
        std::cout << "Parameters are required" << "\n";
        std::cout << "Run with --help for more information." << "\n";
        return EXIT_FAILURE;
    }
    // subcommand which compute and display hash of given file
    if (*hash) {
        std::cout << "Computing hash of " << fileToHash.string() << "\n";
        std::optional<std::array<std::uint64_t, 2>> hashFromFile = Md5FromFile(fileToHash);
        if (!hashFromFile) { // if hash can't be computed
            std::cerr << "Hash could not be computed" << "\n";
            return EXIT_FAILURE;
        }
        std::stringstream ssHex;
        ssHex << std::setw(16) << std::setfill('0') << std::hex << hashFromFile.value()[0];
        ssHex << std::setw(16) << std::setfill('0') << std::hex << hashFromFile.value()[1];
        std::cout << "Result: " << ssHex.str() << "\n";
        return EXIT_SUCCESS;
    }
    // building quarantine directory path
    std::string homedir = getenv("HOME");
    quarantineDirectory = homedir + "/.quarantine";
    std::cout << "Quarantine directory: " << quarantineDirectory.string() << "\n";

    // creating quarantine directory if it doesn't exist
    if (!std::filesystem::exists(quarantineDirectory)) {
        std::cout << "Creating quarantine directory..." << "\n";
        if (!std::filesystem::create_directory(quarantineDirectory)) {
            return EXIT_FAILURE;
        }
    }
    // "opening" quarantine for next operations
    if (!AlterQuarantinePermissions(0700)) {
        return EXIT_FAILURE;
    }
    // subcommand which show files with stats in quarantine
    if (*showQuarantine) {
        ShowFilesInQuarantine();
        AlterQuarantinePermissions(0); // "closing" quarantine
        return EXIT_SUCCESS;
    }

    // subcommand which restore specific file from quarantine
    if (*restore) {
        if (!RestoreFromQuarantine(restoreFilename)) {
            std::cerr << "File from quarantine could not be restored" << "\n";
            AlterQuarantinePermissions(0);
            return EXIT_FAILURE;
        }
        std::cout << "File from quarantine was restored" << "\n";
        AlterQuarantinePermissions(0);
        return EXIT_SUCCESS;
    }
    // subcommand which scan given directory or file using yara
    if (*yara) {
        if (!InitializeYaraDetector(rule_paths)) {
            std::cerr << "Yara detector cannot initialize successfully\n";
            AlterQuarantinePermissions(0);
            return EXIT_FAILURE;
        }
        ScanFiles(target, &ScanUsingYaraDetector);
        AlterQuarantinePermissions(0);
        return EXIT_SUCCESS;
    }

    std::cout << "Loading hashes..." << "\n";
    if (!GetHashesFromFile(hashesSet, hashes)) { // loading hashes
        std::cerr << "Unable to read file containing hashes" << "\n";
        AlterQuarantinePermissions(0);
        return EXIT_FAILURE;
    }

    // subcommand which scan given directory
    if (*scan) {
        std::cout << "Starting scanning:" << "\n";
        ScanFiles(target, &ScanFile);
        std::cout << "Scan has been finished! \n";
        AlterQuarantinePermissions(0);
        return EXIT_SUCCESS;
    }
    // subcommand which monitor changes and scan files
    if (*monitor) {
        if (!MonitorDirectoryRecursively(directoryToMonitor)) {
            std::cerr << "Monitoring has been stopped with error!\n";
            AlterQuarantinePermissions(0);
            return EXIT_FAILURE;
        }
        AlterQuarantinePermissions(0);
        std::cerr << "Monitoring has been finished!\n";
        return EXIT_SUCCESS;
    }

}