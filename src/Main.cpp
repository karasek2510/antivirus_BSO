#include <iostream>
#include <unordered_set>
#include "../headers/MD5.h"
#include "../headers/FileManagement.h"
#include "../headers/Main.h"
#include <tclap/CmdLine.h>
#include <filesystem>
#include "../headers/Scanner.h"

std::unordered_set<std::string> unorderedSet;

int main(int argc, char **argv) {
    try {
//        TCLAP::CmdLine cmd("Antivirus project BSO",' ', "1.0");
//        TCLAP::ValueArg<std::string> directoryArg("T","target","File/directory to scan",true,"null","string");
//        cmd.add(directoryArg);
//        TCLAP::ValueArg<std::string> hashesArg("H","hashes","File containing hashes",true,"null","string");
//        cmd.add(hashesArg);
//        cmd.parse(argc,argv);
        std::string target = "/etc";
        std::string hashes = "/home/karasek/code/antivirus_BSO/resources/bigHashes.txt";
        getFileContent(hashesSet, hashes);
        scanFiles(target);
        std::cout << "END!!! \n";


    } catch (TCLAP::ArgException &argException) {
        std::cerr << "error: " << argException.error() << " for arg " << argException.argId() << std::endl;
    } catch (std::filesystem::__cxx11::filesystem_error &filesystemError) {
        std::cerr << filesystemError.what() << "\n";
    }
}