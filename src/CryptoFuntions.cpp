#include <iostream>
#include <fstream>
#include <iomanip>
#include <optional>

#include <cryptopp/modes.h>
#include <cryptopp/aes.h>
#include <cryptopp/filters.h>
#include <cryptopp/files.h>

#include <cryptopp/md5.h>
#include <cryptopp/cryptlib.h>

#include "../headers/CryptoFuntions.h"

std::array<std::uint64_t,2>  byteArray16ToUint64s(std::array<byte, CryptoPP::MD5::DIGESTSIZE> array){
    std::array<std::uint64_t,2>  result;
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


std::optional<std::array<std::uint64_t,2>> md5FromFile(const std::string& path) {
    CryptoPP::MD5 md5;
    std::array<byte, CryptoPP::MD5::DIGESTSIZE> hashArray;
    try{
        CryptoPP::FileSource fs( path.c_str(), true,
                                 new CryptoPP::HashFilter( md5,
                                                           new CryptoPP::ArraySink(hashArray.data(), CryptoPP::MD5::DIGESTSIZE)
                                 ));
    }
    catch (CryptoPP::FileStore::Err err){
        return std::nullopt;
    }
    std::array<std::uint64_t,2>  hash = byteArray16ToUint64s(hashArray);
    return hash;
}

void encryptAES(const std::array<std::byte, CryptoPP::AES::DEFAULT_KEYLENGTH> &key,
                const std::array<std::byte, CryptoPP::AES::BLOCKSIZE> &iv,
                const std::string &filename_in, const std::string &filename_out) {

    CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption cipher{};
    cipher.SetKeyWithIV(reinterpret_cast<const byte *>(key.data()), key.size(),
                        reinterpret_cast<const byte *>(iv.data()));

    std::ifstream in{filename_in, std::ios::binary};
    std::ofstream out{filename_out, std::ios::binary};


    CryptoPP::FileSource{in, true,
                         new CryptoPP::StreamTransformationFilter{
                                 cipher, new CryptoPP::FileSink{out}}};
}

void decryptAES(const std::array<std::byte, CryptoPP::AES::DEFAULT_KEYLENGTH> &key,
                const std::array<std::byte, CryptoPP::AES::BLOCKSIZE> &iv,
                const std::string &filename_in, const std::string &filename_out) {

    CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption cipher{};
    cipher.SetKeyWithIV(reinterpret_cast<const byte *>(key.data()), key.size(),
                        reinterpret_cast<const byte *>(iv.data()));

    std::ifstream in{filename_in, std::ios::binary};
    std::ofstream out{filename_out, std::ios::binary};

    CryptoPP::FileSource{in, true,
                         new CryptoPP::StreamTransformationFilter{
                                 cipher, new CryptoPP::FileSink{out}}};
}
