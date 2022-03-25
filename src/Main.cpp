#include <iostream>
#include <unordered_set>
#include "../headers/CryptoFuntions.h"
#include "../headers/FileManagement.h"
#include "../headers/Main.h"
#include <tclap/CmdLine.h>
#include <filesystem>
#include <cryptopp/osrng.h>
#include "../headers/Scanner.h"


#include "cryptopp/modes.h"
#include "cryptopp/aes.h"
#include "cryptopp/filters.h"

using aes_key_t = std::array<std::byte, CryptoPP::AES::DEFAULT_KEYLENGTH>;
using aes_iv_t = std::array<std::byte , CryptoPP::AES::BLOCKSIZE>;

std::unordered_set<std::string> unorderedSet;
std::filesystem::path quarantineDirectory;

int main(int argc, char **argv) {
    try {
//        TCLAP::CmdLine cmd("Antivirus project BSO",' ', "1.0");
//        TCLAP::ValueArg<std::string> directoryArg("T","target","File/directory to scan",true,"null","string");
//        cmd.add(directoryArg);
//        TCLAP::ValueArg<std::string> hashesArg("H","hashes","File containing hashes",true,"null","string");
//        cmd.add(hashesArg);
//        cmd.parse(argc,argv);
//        std::string target = "/home";
//        std::string hashes = "/home/karasek/code/antivirus_BSO/resources/bigHashes.txt";
//        getFileContent(hashesSet, hashes);
//        scanFiles(target);
//        std::cout << "END!!! \n";


        std::cout <<  CryptoPP::AES::BLOCKSIZE << std::endl;

        CryptoPP::AutoSeededRandomPool rng{};

        // Generate a random key
        aes_key_t key{};
        rng.GenerateBlock(reinterpret_cast<byte *>(key.data()), key.size());

        // Generate a random IV
        aes_iv_t iv{};
        rng.GenerateBlock(reinterpret_cast<byte *>(iv.data()), iv.size());
        auto start = std::chrono::steady_clock::now();
        // encrypt
        encryptAES(key, iv, "/home/karasek/Downloads/1GB.bin", "/home/karasek/Downloads/1GBEn.bin");
        std::cout << "Elapsed time in mili: "
             << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count()
             << " ns" << "\n";
        // decrypt
        decryptAES(key, iv, "/home/karasek/Downloads/1GBEn.bin", "/home/karasek/Downloads/1GGDen.bin");
        std::cout << "Elapsed time in mili: "
                  << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count()
                  << " ns" << "\n";
        return 0;


    } catch (TCLAP::ArgException &argException) {
        std::cerr << "error: " << argException.error() << " for arg " << argException.argId() << std::endl;
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << "\n";
    }
}