#include <iostream>
#include <unordered_set>
#include "../headers/MD5.h"
#include "../headers/FileManagement.h"
#include <tclap/CmdLine.h>
#include <filesystem>

int main(int argc, char **argv) {
    try{
        TCLAP::CmdLine cmd("Antivirus project BSO",' ', "1.0");
        TCLAP::ValueArg<std::string> directoryArg("T","target","File/directory to scan",true,"null","string");
        cmd.add(directoryArg);
        TCLAP::ValueArg<std::string> hashesArg("H","hashes","File containing hashes",true,"null","string");
        cmd.add(hashesArg);
        cmd.parse(argc,argv);
        std::string directory = directoryArg.getValue();
        std::string hashes = hashesArg.getValue();
        std::unordered_set<std::string> hashesSet;
        getFileContent(hashesSet,hashes);
        std::cout << "Hashes: " << hashesSet.size() << "\n";
        std::vector<std::string> files = listFiles(directory);
        std::string md5Hash;
        std::cout << "Files: "<< files.size() << "\n";
        for(const std::string& file : files) {

        }

    }catch (TCLAP::ArgException& argException){
        std::cerr << "error: " << argException.error() << " for arg " << argException.argId() << std::endl;
    }catch (std::filesystem::__cxx11::filesystem_error& filesystemError){
        std::cerr << "An invalid file name was specified" << "\n";
    }
}