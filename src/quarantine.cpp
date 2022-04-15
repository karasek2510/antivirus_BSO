#include "../headers/quarantine.h"

#include <filesystem>
#include <iostream>
#include <iterator>
#include <vector>

#include <cryptopp/aes.h>
#include <cryptopp/osrng.h>

#include "../headers/crypto_functions.h"
#include "../headers/main.h"

// using global variable from Main.h
std::filesystem::path quarantineDirectory;

// computing new name of the file before adding to quarantine to avoid duplications
std::filesystem::path GetFullPathQuarantine(const std::filesystem::path &file, const std::filesystem::path &directory) {
    std::string baseFilename = file.stem();
    std::string fileExtension = file.extension();
    int counter = 0;
    std::string fileInDirectory;
    std::string tempFileInDirectory;
    tempFileInDirectory.append(directory).append("/").append(baseFilename).append("_");
    try {
        do {
            fileInDirectory = tempFileInDirectory;
            fileInDirectory.append(std::to_string(counter)).append(fileExtension);
            counter++;
        } while (std::filesystem::exists(fileInDirectory));
    } catch (std::exception &ex) {
        std::cerr << "Unable to open quarantine directory \n";
        return "";
    }
    return fileInDirectory;
}

// generate info file for specific file in quarantine;
// contains:
//              - filename in quarantine
//              - location of the file before moving to quarantine
//              - permissions
//              - key and iv used in AES
//              - time of moving file to quarantine
template<std::size_t SIZE_KEY, std::size_t SIZE_IV>
bool GenerateInfoFile(const std::filesystem::path &infoFilePath, const std::string &filename,
                      const std::filesystem::path &originalLocation,
                      std::filesystem::perms perms, const std::array<std::byte, SIZE_KEY> &key,
                      const std::array<std::byte, SIZE_IV> &iv) {

    std::stringstream fileContent;
    std::string originalFilename = originalLocation.filename();
    std::string parentPath;
    try {
        parentPath = std::filesystem::canonical(originalLocation.parent_path());
    } catch (std::filesystem::filesystem_error &ex) {
        std::cerr << "Cannot resolve parent path \n";
        return false;
    }
    long t = std::time(nullptr);
    tm tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%d-%m-%Y_%H-%M-%S");
    fileContent << "Filename: " << filename << "\n";
    fileContent << "Original location: " << parentPath.append("/").append(originalFilename) << "\n";
    fileContent << "Permissions: " << static_cast<int>(perms) << "\n";
    fileContent << "Key: " << ArrayToHexString<CryptoPP::AES::DEFAULT_KEYLENGTH>(key) << "\n";
    fileContent << "IV: " << ArrayToHexString<CryptoPP::AES::BLOCKSIZE>(iv) << "\n";
    fileContent << "Creation time: " << oss.str() << "\n";
    std::ofstream outfile(infoFilePath);
    if (!outfile) {
        return false;
    }
    outfile << fileContent.str();
    outfile.close();
    return true;
}

// moving file to quarantine
bool DoQuarantine(const std::filesystem::path &path) {
    std::filesystem::path fullPathInQuarantine = GetFullPathQuarantine(path, quarantineDirectory);
    if (fullPathInQuarantine == "") {
        return false;
    }
    std::filesystem::perms perms = std::filesystem::status(path).permissions(); // saving file permissions

    CryptoPP::AutoSeededRandomPool rng{}; // object used to generate random key and iv

    std::array<std::byte, CryptoPP::AES::DEFAULT_KEYLENGTH> key{};
    rng.GenerateBlock(reinterpret_cast<byte *>(key.data()), key.size());
    std::array<std::byte, CryptoPP::AES::BLOCKSIZE> iv{};
    rng.GenerateBlock(reinterpret_cast<byte *>(iv.data()), iv.size());

    std::cout << "Encrypting file..." << "\n";
    if (!EncryptAES(key, iv, path, fullPathInQuarantine)) {
        return false;
    }
    std::cout << path.string() << " was copied to " << fullPathInQuarantine.string() << "\n";

    if (std::remove(path.c_str()) != 0) {
        std::cerr << "Cannot remove file" << '\n';
        return false;
    }

    std::string infoFilePath;
    infoFilePath.append(quarantineDirectory).append("/.")
            .append(fullPathInQuarantine.filename()).append(".info"); // building infoFile path

    bool infoStatus = GenerateInfoFile<CryptoPP::AES::DEFAULT_KEYLENGTH, CryptoPP::AES::BLOCKSIZE>(infoFilePath,
                                                                                                   fullPathInQuarantine.filename(),
                                                                                                   path, perms, key,
                                                                                                   iv);
    if (!infoStatus) {
        std::cerr << "Cannot generate info file" << '\n';
        return false;
    }
    std::cout << "Generated info file: " << infoFilePath << "\n";
    return true;
}

// restoring file from quarantine
bool RestoreFromQuarantine(const std::filesystem::path &filename) {
    std::filesystem::path fileToRestorePath = quarantineDirectory.string().append("/").append(filename);
    std::cout << "File to be restored: " << fileToRestorePath.string() << "\n";
    std::filesystem::path infoFilePath = quarantineDirectory.string().append("/.")
            .append(filename.filename().string()).append(".info");
    std::cout << "Info file: " << infoFilePath.string() << "\n";
    std::ifstream infile(infoFilePath);
    if (!infile) {
        std::cerr << "Cannot open info file" << '\n';
        return false;
    }
    std::string line;
    std::vector<std::string> data;
    while (std::getline(infile, line)) { // reading info file
        std::stringstream ss(line);
        std::istream_iterator<std::string> begin(ss);
        std::istream_iterator<std::string> end;
        std::vector<std::string> tokens(begin, end);
        data.push_back(tokens.back());
    }
    std::filesystem::path originalLocationPath = data.at(1);
    int perms = static_cast<int>(stol(data.at(2)));
    std::array<std::byte, CryptoPP::AES::DEFAULT_KEYLENGTH> tempKey = HexStringToArray<CryptoPP::AES::DEFAULT_KEYLENGTH>(
            data.at(3));
    std::array<std::byte, CryptoPP::AES::BLOCKSIZE> tempIV = HexStringToArray<CryptoPP::AES::BLOCKSIZE>(data.at(4));
    std::cout << "Decrypting and moving file to: " << originalLocationPath << "\n";
    if (!DecryptAES(tempKey, tempIV, fileToRestorePath, originalLocationPath)) {
        return false;
    }
    try {
        std::filesystem::permissions(originalLocationPath, static_cast<std::filesystem::perms>(perms));
    } catch (std::filesystem::filesystem_error &ex) {
        std::cerr << "Cannot recover permissions\n";
    }
    if (!(std::filesystem::remove(infoFilePath) && std::filesystem::remove(fileToRestorePath))) {
        std::cerr << "Cannot remove obsolete files from quarantine directory" << '\n';
    }
    return true;
}

// showing file in quarantine with simple stats
void ShowFilesInQuarantine() {
    for (const auto &file: std::filesystem::directory_iterator(quarantineDirectory)) {
        if (file.path().filename().c_str()[0] == '.') {
            std::ifstream infile(file.path());
            if (!infile) {
                std::cerr << "Cannot open " << file.path().string() << '\n';
            }
            std::string line;
            std::vector<std::string> data;
            while (std::getline(infile, line)) {
                std::stringstream ss(line);
                std::istream_iterator<std::string> begin(ss);
                std::istream_iterator<std::string> end;
                std::vector<std::string> tokens(begin, end);
                data.push_back(tokens.back());
            }
            std::cout << data.at(0) << ":\n"; // Filename
            std::cout << "\tOriginal Location: " << data.at(1) << "\n";
            std::cout << "\tCreation time: " << data.at(5) << "\n";
        }
    }
}

// altering permissions on quarantine directory
bool AlterQuarantinePermissions(int perms) {
    try {
        std::filesystem::permissions(quarantineDirectory, static_cast<std::filesystem::perms>(perms));
    } catch (std::filesystem::filesystem_error &ex) {
        std::cerr << "Cannot change quarantine directory permissions \n";
        return false;
    }
    return true;
}

