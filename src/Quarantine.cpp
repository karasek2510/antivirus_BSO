//
// Created by karasek on 3/24/22.
//
#include <iostream>
#include <filesystem>
#include <vector>
#include <cryptopp/osrng.h>
#include <fstream>
#include <iterator>
#include "../headers/Quarantine.h"
#include "../headers/FileManagement.h"
#include "../headers/CryptoFuntions.h"

extern std::filesystem::path quarantineDirectory;

std::string vectorToHexString(const std::vector<std::byte>& vector) {
    std::ostringstream ss;
    ss << std::hex << std::uppercase << std::setfill('0');
    ss << std::hex << std::uppercase << std::setfill('0');
    for (std::byte b: vector) {
        ss << std::setw(2) << static_cast<int>(b);
    }

    return ss.str();
}

std::vector<std::byte> hexStringToVector(const std::string &hex) {
    std::vector<std::byte> bytes;
    for (unsigned int i = 0; i < hex.length(); i += 2) {
        std::string byteString = hex.substr(i, 2);
        auto byte = static_cast<std::byte>(strtol(byteString.c_str(), nullptr, 16));
        bytes.push_back(byte);
    }
    return bytes;
}

bool generateInfoFile(const std::filesystem::path& infoFilePath, const std::string& filename, const std::filesystem::path& originalLocation,
                      const std::vector<std::byte>& key, const std::vector<std::byte>& iv) {

    std::stringstream fileContent;
    fileContent << "Filename: " << filename << "\n";
    fileContent << "Original location: " << originalLocation.string() << "\n";
    fileContent << "Key: " << vectorToHexString(key) << "\n";
    fileContent << "IV: " << vectorToHexString(iv) << "\n";
    std::ofstream outfile(infoFilePath);
    if (!outfile) {
        return false;
    }
    outfile << fileContent.str();
    outfile.close();
    return true;
}

bool doQuarantine(const std::filesystem::path& path) {
    std::filesystem::path fullPathInQuarantine = getFullPathQuarantine(path, quarantineDirectory);

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
    infoFilePath.append(quarantineDirectory).append("/")
            .append(fullPathInQuarantine.filename()).append(".info");

    bool infoStatus = generateInfoFile(infoFilePath, fullPathInQuarantine.filename(), path,
                                       std::vector<std::byte>(key.begin(), key.end()),
                                       std::vector<std::byte>(iv.begin(), iv.end()));
    if (!infoStatus) {
        std::cerr << "Cannot generate info file" << '\n';
        return false;
    }
    std::cout << "Generated info file: " << infoFilePath << "\n";
    return true;
}

bool restoreFromQuarantine(const std::filesystem::path& filename) {
    std::filesystem::path fileToRestorePath = quarantineDirectory.string().append("/").append(filename);
    std::cout << "File to be restored: " << fileToRestorePath.string() << "\n";
    std::filesystem::path infoFilePath = quarantineDirectory.string().append("/")
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
    std::vector<std::byte> tempKey = hexStringToVector(data.at(2));
    std::vector<std::byte> tempIV = hexStringToVector(data.at(3));
    std::array<std::byte, CryptoPP::AES::DEFAULT_KEYLENGTH> key{};
    std::array<std::byte, CryptoPP::AES::BLOCKSIZE> iv{};
    std::copy(tempKey.begin(), tempKey.begin() + CryptoPP::AES::DEFAULT_KEYLENGTH, key.begin());
    std::copy(tempIV.begin(), tempIV.begin() + CryptoPP::AES::BLOCKSIZE, iv.begin());
    std::cout << "Decrypting and moving file to: " << originalLocationPath << "\n";
    decryptAES(key, iv, fileToRestorePath, originalLocationPath);
    return true;

}

