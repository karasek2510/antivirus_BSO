#include <iostream>
#include <unordered_set>
#include "../headers/CryptoFuntions.h"
#include "../headers/FileManagement.h"
#include "../headers/Main.h"
#include <tclap/CmdLine.h>
#include <filesystem>
#include <cryptopp/osrng.h>
#include "../headers/Scanner.h"
#include "../headers/Quarantine.h"


#include "cryptopp/modes.h"
#include "cryptopp/aes.h"
#include "cryptopp/filters.h"


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
//        std::string target = "/home/karasek/code/antivirus_BSO/resources/hashes2.txt";
//        std::string hashes = "/home/karasek/code/antivirus_BSO/resources/bigHashes.txt";
        quarantineDirectory = "/home/karasek/Q";
//        if(!std::filesystem::exists(quarantineDirectory)){
//            std::filesystem::create_directory(quarantineDirectory);
//        }
//        getFileContent(hashesSet, hashes);
//        scanFiles(target);
//        std::cout << "END!!! \n";
        restoreFromQuarantine("hashes_7.txt");
        return 0;


    } catch (TCLAP::ArgException &argException) {
        std::cerr << "error: " << argException.error() << " for arg " << argException.argId() << std::endl;
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << "\n";
    }
}