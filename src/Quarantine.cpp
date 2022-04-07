
#include <filesystem>
#include <iostream>
#include <iterator>
#include <vector>

#include <cryptopp/osrng.h>

#include "../headers/CryptoFuntions.h"
#include "../headers/FileManagement.h"
#include "../headers/Quarantine.h"

extern std::filesystem::path quarantineDirectory;

template<std::size_t SIZE>
std::string arrayToHexString(const std::array<std::byte, SIZE> &array) {
    std::ostringstream ss;
    ss << std::hex << std::uppercase << std::setfill('0');
    ss << std::hex << std::uppercase << std::setfill('0');
    for (std::byte b: array) {
        ss << std::setw(2) << static_cast<int>(b);
    }
    return ss.str();
}

template<std::size_t SIZE>
std::array<std::byte, SIZE> hexStringToArray(const std::string &hex) {
    std::array<std::byte, SIZE> bytes;
    for (unsigned int i = 0; i < hex.length(); i += 2) {
        std::string byteString = hex.substr(i, 2);
        auto byte = static_cast<std::byte>(strtol(byteString.c_str(), nullptr, 16));
        bytes[i] = byte;
    }
    return bytes;
}

template<std::size_t SIZE_KEY, std::size_t SIZE_IV>
bool generateInfoFile(const std::filesystem::path &infoFilePath, const std::string &filename,
                      const std::filesystem::path &originalLocation,
                      std::filesystem::perms perms, const std::array<std::byte, SIZE_KEY> &key,
                      const std::array<std::byte, SIZE_IV> &iv) {

    std::stringstream fileContent;
    fileContent << "Filename: " << filename << "\n";
    fileContent << "Original location: " << originalLocation.string() << "\n";
    fileContent << "Permissions: " << static_cast<int>(perms) << "\n";
    fileContent << "Key: " << arrayToHexString<CryptoPP::AES::DEFAULT_KEYLENGTH>(key) << "\n";
    fileContent << "IV: " << arrayToHexString<CryptoPP::AES::BLOCKSIZE>(iv) << "\n";
    std::ofstream outfile(infoFilePath);
    if (!outfile) {
        return false;
    }
    outfile << fileContent.str();
    outfile.close();
    return true;
}

bool doQuarantine(const std::filesystem::path &path) {
    std::filesystem::path fullPathInQuarantine = getFullPathQuarantine(path, quarantineDirectory);

    std::filesystem::perms perms = std::filesystem::status(path).permissions();

    CryptoPP::AutoSeededRandomPool rng{};

    // Generate a random Key
    std::array<std::byte, CryptoPP::AES::DEFAULT_KEYLENGTH> key{};
    rng.GenerateBlock(reinterpret_cast<byte *>(key.data()), key.size());

    // Generate a random IV
    std::array<std::byte, CryptoPP::AES::BLOCKSIZE> iv{};
    rng.GenerateBlock(reinterpret_cast<byte *>(iv.data()), iv.size());

    std::cout << "Encrypting file..." << "\n";
    encryptAES(key, iv, path, fullPathInQuarantine);

    std::cout << path.string() << " was copied to " << fullPathInQuarantine.string() << "\n";
    try {
//        std::filesystem::remove(path);                        COMMENT FOR TESTING
    } catch (std::filesystem::filesystem_error &e) {
        std::cerr << "Cannot remove file" << '\n';
        return false;
    }

    std::string infoFilePath;
    infoFilePath.append(quarantineDirectory).append("/.")
            .append(fullPathInQuarantine.filename()).append(".info");

    bool infoStatus = generateInfoFile<CryptoPP::AES::DEFAULT_KEYLENGTH, CryptoPP::AES::BLOCKSIZE>(infoFilePath,
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

bool restoreFromQuarantine(const std::filesystem::path &filename) {
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
    while (std::getline(infile, line)) {
        std::stringstream ss(line);
        std::istream_iterator<std::string> begin(ss);
        std::istream_iterator<std::string> end;
        std::vector<std::string> tokens(begin, end);
        data.push_back(tokens.back());
    }
    std::filesystem::path originalLocationPath = data.at(1);
    int perms = stol(data.at(2));
    std::array<std::byte, CryptoPP::AES::DEFAULT_KEYLENGTH> tempKey = hexStringToArray<CryptoPP::AES::DEFAULT_KEYLENGTH>(
            data.at(3));
    std::array<std::byte, CryptoPP::AES::BLOCKSIZE> tempIV = hexStringToArray<CryptoPP::AES::BLOCKSIZE>(data.at(4));
    std::cout << "Decrypting and moving file to: " << originalLocationPath << "\n";
    decryptAES(tempKey, tempIV, fileToRestorePath, originalLocationPath);
    std::filesystem::permissions(originalLocationPath, static_cast<std::filesystem::perms>(perms));
    if (!(std::filesystem::remove(infoFilePath) && std::filesystem::remove(fileToRestorePath))) {
        std::cerr << "Cannot remove obsolete files from quarantine directory" << '\n';
    }
    return true;

}

