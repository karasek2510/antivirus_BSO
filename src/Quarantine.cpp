#include "../headers/Quarantine.h"

#include <filesystem>
#include <iostream>
#include <iterator>
#include <vector>

#include <cryptopp/osrng.h>

#include "../headers/CryptoFuntions.h"

extern std::filesystem::path quarantineDirectory;

std::filesystem::path getFullPathQuarantine(const std::filesystem::path &file, const std::filesystem::path &directory) {
    std::string baseFilename = file.stem();
    std::string fileExtension = file.extension();
    int counter = 0;
    std::string fileInDirectory;
    std::string tempFileInDirectory;
    tempFileInDirectory.append(directory).append("/").append(baseFilename).append("_");
    try{
        do {
            fileInDirectory = tempFileInDirectory;
            fileInDirectory.append(std::to_string(counter)).append(fileExtension);
            counter++;
        } while (std::filesystem::exists(fileInDirectory));
    }catch (std::exception &ex){
        std::cerr << "Unable to open quarantine directory \n";
        return "";
    }
    return fileInDirectory;
}

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
        bytes[i/2] = byte;
    }
    return bytes;
}

template<std::size_t SIZE_KEY, std::size_t SIZE_IV>
bool generateInfoFile(const std::filesystem::path &infoFilePath, const std::string &filename,
                      const std::filesystem::path &originalLocation,
                      std::filesystem::perms perms, const std::array<std::byte, SIZE_KEY> &key,
                      const std::array<std::byte, SIZE_IV> &iv) {

    std::stringstream fileContent;
    std::string originalFilename = originalLocation.filename();
    std::string parentPath;
    try{
        parentPath = std::filesystem::canonical(originalLocation.parent_path());
    }catch (std::filesystem::filesystem_error& ex){
        std::cerr << "Cannot resolve parent path \n";
        return false;
    }
    long t = std::time(nullptr);
    tm tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%d-%m-%Y_%H-%M-%S");
    fileContent << "Filename: " << filename << "\n";
    fileContent << "Original location: " <<  parentPath.append("/").append(originalFilename)<< "\n";
    fileContent << "Permissions: " << static_cast<int>(perms) << "\n";
    fileContent << "Key: " << arrayToHexString<CryptoPP::AES::DEFAULT_KEYLENGTH>(key) << "\n";
    fileContent << "IV: " << arrayToHexString<CryptoPP::AES::BLOCKSIZE>(iv) << "\n";
    fileContent << "Creation time: " << oss.str() << "\n";
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
    if(fullPathInQuarantine==""){
        return false;
    }
    std::filesystem::perms perms = std::filesystem::status(path).permissions();

    CryptoPP::AutoSeededRandomPool rng{};

    std::array<std::byte, CryptoPP::AES::DEFAULT_KEYLENGTH> key{};
    rng.GenerateBlock(reinterpret_cast<byte *>(key.data()), key.size());
    std::array<std::byte, CryptoPP::AES::BLOCKSIZE> iv{};
    rng.GenerateBlock(reinterpret_cast<byte *>(iv.data()), iv.size());

    std::cout << "Encrypting file..." << "\n";
    encryptAES(key, iv, path, fullPathInQuarantine);

    std::cout << path.string() << " was copied to " << fullPathInQuarantine.string() << "\n";

    if(std::remove(path.c_str())!=0){
        std::cerr << "Cannot remove file" << '\n';
        return false;
    }

    std::string infoFilePath;
    infoFilePath.append(quarantineDirectory).append("/.")
            .append(fullPathInQuarantine.filename()).append(".info");

    bool infoStatus = generateInfoFile<CryptoPP::AES::DEFAULT_KEYLENGTH, CryptoPP::AES::BLOCKSIZE>(infoFilePath,
                                                                                                   fullPathInQuarantine.filename(),
                                                                                                   path, perms, key,iv);
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
    int perms = static_cast<int>(stol(data.at(2)));
    std::array<std::byte, CryptoPP::AES::DEFAULT_KEYLENGTH> tempKey = hexStringToArray<CryptoPP::AES::DEFAULT_KEYLENGTH>(
            data.at(3));
    std::array<std::byte, CryptoPP::AES::BLOCKSIZE> tempIV = hexStringToArray<CryptoPP::AES::BLOCKSIZE>(data.at(4));
    std::cout << "Decrypting and moving file to: " << originalLocationPath << "\n";
    decryptAES(tempKey, tempIV, fileToRestorePath, originalLocationPath);
    try{
        std::filesystem::permissions(originalLocationPath, static_cast<std::filesystem::perms>(perms));
    }catch (std::filesystem::filesystem_error& ex){
        std::cerr << "Cannot recover permissions\n";
    }
    if (!(std::filesystem::remove(infoFilePath) && std::filesystem::remove(fileToRestorePath))) {
        std::cerr << "Cannot remove obsolete files from quarantine directory" << '\n';
    }
    return true;
}

void showFilesInQuarantine(){
    for(const auto &file: std::filesystem::directory_iterator(quarantineDirectory)){
        if(file.path().filename().c_str()[0]=='.'){
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

bool alterQuarantinePermissions(int perms){
    try{
        std::filesystem::permissions(quarantineDirectory, static_cast<std::filesystem::perms>(perms));
    }catch (std::filesystem::filesystem_error& ex){
        std::cerr << "Cannot change quarantine directory permissions \n";
        return false;
    }
    return true;
}

