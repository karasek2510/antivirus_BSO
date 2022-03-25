//
// Created by karasek on 3/24/22.
//
#include <iostream>
#include <filesystem>
#include <vector>
#include <cryptopp/osrng.h>
#include "../headers/Quarantine.h"
#include "../headers/FileManagement.h"
#include "../headers/CryptoFuntions.h"

extern std::filesystem::path quarantineDirectory;

void generateInfoFile(std::filesystem::path infoFilePath,std::string filename, std::filesystem::path originalLocation,std::vector<std::byte> key, std::vector<std::byte> iv){

}

void doQuarantine(std::filesystem::path path){
    std::string fullPathInQuarantine = getFullPathQuarantine(path,quarantineDirectory);
    std::cout << fullPathInQuarantine << "\n";
    try {
        std::filesystem::copy(path, fullPathInQuarantine); // WILL BE MOVE
    } catch (std::filesystem::filesystem_error& e) {
        std::cout << e.what() << '\n';
    }
    CryptoPP::AutoSeededRandomPool rng{};

    // Generate a random Key
    std::array<std::byte, CryptoPP::AES::DEFAULT_KEYLENGTH> key{};
    rng.GenerateBlock(reinterpret_cast<byte *>(key.data()), key.size());

    // Generate a random IV
    std::array<std::byte , CryptoPP::AES::BLOCKSIZE> iv{};
    rng.GenerateBlock(reinterpret_cast<byte *>(iv.data()), iv.size());

    encryptAES(key, iv, path, fullPathInQuarantine);

    infoFile
    generateInfoFile()
}

