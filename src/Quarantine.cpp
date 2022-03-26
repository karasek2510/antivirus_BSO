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

std::string vectorToHexString(std::vector<std::byte> vector){
    std::ostringstream ss;
    ss << std::hex << std::uppercase << std::setfill( '0' );
    ss << std::hex << std::uppercase << std::setfill( '0' );
    for( std::byte b : vector ) {
        ss << std::setw( 2 ) << static_cast<int>(b);
    }

    return ss.str();
}

std::vector<std::byte> hexStringToVector(const std::string& hex) {
    std::vector<std::byte> bytes;
    for (unsigned int i = 0; i < hex.length(); i += 2) {
        std::string byteString = hex.substr(i, 2);
        std::byte byte = static_cast<std::byte>(strtol(byteString.c_str(), NULL, 16));
        bytes.push_back(byte);
    }
    return bytes;
}

void generateInfoFile(std::string filename, std::filesystem::path originalLocation,std::vector<std::byte> key, std::vector<std::byte> iv){
    std::string infoFilePath;
    infoFilePath.append(quarantineDirectory).append("/")
            .append(filename).append(".info");
    std::stringstream fileContent;
    fileContent << "Filename: " << filename << "\n";
    fileContent << "Original location: " << originalLocation.string() << "\n";
    fileContent << "Key: " << vectorToHexString(key) << "\n";
    fileContent << "IV: " << vectorToHexString(iv) << "\n";
    std::ofstream outfile (infoFilePath);
    outfile << fileContent.str();
    outfile.close();
}

void doQuarantine(std::filesystem::path path){
    std::filesystem::path fullPathInQuarantine = getFullPathQuarantine(path,quarantineDirectory);
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

    generateInfoFile(fullPathInQuarantine.filename(), path,
                        std::vector<std::byte>(key.begin(),key.end()),
                        std::vector<std::byte>(iv.begin(),iv.end()));
}

void restoreFromQuarantine(std::filesystem::path filename){
    std::filesystem::path infoFilePath = quarantineDirectory.string().append("/")
            .append(filename.stem().string()).append(".info");
    std::ifstream infile(infoFilePath);
    std::string line;
    std::vector<std::string> data;
    while (std::getline(infile, line)){
        std::stringstream ss(line);
        std::istream_iterator<std::string> begin(ss);
        std::istream_iterator<std::string> end;
        std::vector<std::string> tokens(begin, end);
        data.push_back(tokens.back());
    }
    std::filesystem::path originalLocationPath = data.at(1);
    std::vector<std::byte> tempKey = hexStringToVector(data.at(2));
    std::vector<std::byte> tempIV = hexStringToVector(data.at(3));
    std::array<std::byte,CryptoPP::AES::DEFAULT_KEYLENGTH> key{};
    std::array<std::byte,CryptoPP::AES::BLOCKSIZE> iv{};
    std::copy(tempKey.begin(), tempKey.begin()+CryptoPP::AES::DEFAULT_KEYLENGTH, key.begin());
    std::copy(tempIV.begin(), tempIV.begin()+CryptoPP::AES::BLOCKSIZE, iv.begin());
    std::filesystem::path fileToRestorePath = quarantineDirectory.string().append("/").append(filename);
    decryptAES(key,iv,fileToRestorePath,originalLocationPath);
    std::cout << "DOOOONE"<< "\n";

}

