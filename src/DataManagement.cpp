#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1

#include "../headers/DataManagement.h"

#include <sys/vfs.h>

#include <filesystem>
#include <string>
#include <unordered_set>

#include <cryptopp/md5.h>

std::array<std::uint64_t, 2> StringHashToUint64s(const std::string &strHash) {
    std::uint64_t value1 = std::stoull(strHash.substr(0,16),nullptr, 16);
    std::uint64_t value2 = std::stoull(strHash.substr(16,16),nullptr, 16);
    return std::array<std::uint64_t, 2>{value1, value2};
}

int CheckFileFs(const std::filesystem::path &path) {
    struct statfs fileFs{};
    if (statfs(path.c_str(), &fileFs) != 0) {
        return -1;
    }
    return static_cast<int>(fileFs.f_type);
}

std::array<std::uint64_t, 2> ByteArray16ToUint64s(std::array<byte, CryptoPP::Weak1::MD5::DIGESTSIZE> array) {
    std::array<std::uint64_t, 2> result{};
    uint64_t value1 =
            static_cast<uint64_t>(array[7]) |
            static_cast<uint64_t>(array[6]) << 8 |
            static_cast<uint64_t>(array[5]) << 16 |
            static_cast<uint64_t>(array[4]) << 24 |
            static_cast<uint64_t>(array[3]) << 32 |
            static_cast<uint64_t>(array[2]) << 40 |
            static_cast<uint64_t>(array[1]) << 48 |
            static_cast<uint64_t>(array[0]) << 56;
    uint64_t value2 =
            static_cast<uint64_t>(array[15]) |
            static_cast<uint64_t>(array[14]) << 8 |
            static_cast<uint64_t>(array[13]) << 16 |
            static_cast<uint64_t>(array[12]) << 24 |
            static_cast<uint64_t>(array[11]) << 32 |
            static_cast<uint64_t>(array[10]) << 40 |
            static_cast<uint64_t>(array[9]) << 48 |
            static_cast<uint64_t>(array[8]) << 56;
    result[0] = value1;
    result[1] = value2;
    return result;
}

