#include <iostream>
#include <fstream>
#include <iomanip>
#include <optional>

#include "cryptopp/modes.h"
#include "cryptopp/aes.h"
#include "cryptopp/filters.h"
#include <cryptopp/files.h>

#include <openssl/md5.h>

#include "../headers/CryptoFuntions.h"


std::optional<std::string> md5FromFile(const std::string &filename) {
    MD5_CTX context;
    if (!MD5_Init(&context)) {
        return std::nullopt;
    }
    char buf[K_READ_BUF_SIZE];
    std::ifstream file(filename, std::ifstream::binary);
    if (file.good()) {
        while (file.good()) {
            file.read(buf, sizeof(buf));
            if (!MD5_Update(&context, buf, file.gcount())) {
                return std::nullopt;
            }
        }
        unsigned char result[MD5_DIGEST_LENGTH];
        if (!MD5_Final(result, &context)) {
            return std::nullopt;
        }
        std::stringstream md5str;
        md5str << std::hex << std::setfill('0');
        for (const auto &byte: result) {
            md5str << std::setw(2) << (int) byte;
        }
        return md5str.str();
    } else {
        return std::nullopt;
    }
}

void encryptAES(const std::array<std::byte, CryptoPP::AES::DEFAULT_KEYLENGTH> &key,
                const std::array<std::byte, CryptoPP::AES::BLOCKSIZE> &iv,
                const std::string &filename_in, const std::string &filename_out) {

    CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption cipher{};
    cipher.SetKeyWithIV(reinterpret_cast<const byte *>(key.data()), key.size(),
                        reinterpret_cast<const byte *>(iv.data()));

    std::ifstream in{filename_in, std::ios::binary};
    std::ofstream out{filename_out, std::ios::binary};

    CryptoPP::FileSource{in, /*pumpAll=*/true,
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
