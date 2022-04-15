
#ifndef ANTIVIRUS_BSO_FILEMANAGEMENT_H
#define ANTIVIRUS_BSO_FILEMANAGEMENT_H

#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1

#include <filesystem>
#include <unordered_set>

#include <cryptopp/md5.h>

// custom class to implement hash function for std::array of uint64_t
class HashArrayUint64_t {
public:
    size_t operator()(const std::array<std::uint64_t, 2> &arr) const {
        return arr[0];
    }
};

std::array<std::uint64_t, 2> StringHashToUint64s(const std::string& strHash);

int CheckFileFs(const std::filesystem::path& path);

std::array<std::uint64_t, 2> ByteArray16ToUint64s(std::array<byte, CryptoPP::Weak1::MD5::DIGESTSIZE> array);

template<std::size_t SIZE>
std::string ArrayToHexString(const std::array<std::byte, SIZE>& array){
    std::ostringstream ss;
    ss << std::hex << std::uppercase << std::setfill('0');
    ss << std::hex << std::uppercase << std::setfill('0');
    for (std::byte b: array) {
        ss << std::setw(2) << static_cast<int>(b);
    }
    return ss.str();
}

template<std::size_t SIZE>
std::array<std::byte, SIZE> HexStringToArray(const std::string &hex) {
    std::array<std::byte, SIZE> bytes;
    for (unsigned int i = 0; i < hex.length(); i += 2) {
        std::string byteString = hex.substr(i, 2);
        auto byte = static_cast<std::byte>(strtol(byteString.c_str(), nullptr, 16));
        bytes[i/2] = byte;
    }
    return bytes;
}

#endif //ANTIVIRUS_BSO_FILEMANAGEMENT_H