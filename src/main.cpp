#include <iostream>
#include <unordered_set>
#include "../headers/MD5.h"
#include "../headers/FileManagement.h"
#include <tclap/CmdLine.h>
#include <filesystem>

using namespace TCLAP;
using namespace std;


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
        std::vector<std::string> files = listFiles(directory);
        std::string md5Hash;
        for(const std::string& file : files) {
            md5Hash = md5FromFile(file).value_or("Cant evaluate hash!");
//            std::cout << file << " : " << md5Hash << "\n";
            if(isStrInUnorderedSet(hashesSet,md5Hash)) {
                std::cout << "Match: " << file << "\n";
                removeExec(file);
                string homedir = getenv("HOME");
                std::filesystem::create_directory(homedir+"/.danger");
                std::string baseFilename = file.substr(file.find_last_of("/\\") + 1);
                try {
                    std::filesystem::copy(file, homedir.append("/.danger/").append(baseFilename));
                    std::filesystem::remove(file);
                } catch (std::filesystem::filesystem_error& e) {
                    std::cerr << e.what() << '\n';
                }
            }
        }

    }catch (TCLAP::ArgException& argException){
        std::cerr << "error: " << argException.error() << " for arg " << argException.argId() << std::endl;
    }catch (std::filesystem::__cxx11::filesystem_error& filesystemError){
        std::cerr << "An invalid file name was specified" << "\n";
    }
}