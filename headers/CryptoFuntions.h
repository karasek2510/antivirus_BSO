//
// Created by karasek on 3/10/22.
//

#ifndef ANT_CryptoFunctions_H
#define ANT_CryptoFunctions_H

#include <vector>
#include <optional>
#include <filesystem>

#include "cryptopp/modes.h"
#include "cryptopp/aes.h"
#include "cryptopp/filters.h"


static const int K_READ_BUF_SIZE{1024 * 16};

std::optional<std::string> md5FromFile(const std::string &file);

void encryptAES(const std::array<std::byte, CryptoPP::AES::DEFAULT_KEYLENGTH> &key,
                const std::array<std::byte, CryptoPP::AES::BLOCKSIZE> &iv,
                const std::string &filename_in, const std::string &filename_out);

void decryptAES(const std::array<std::byte, CryptoPP::AES::DEFAULT_KEYLENGTH> &key,
                const std::array<std::byte, CryptoPP::AES::BLOCKSIZE> &iv,
                const std::string &filename_in, const std::string &filename_out);


#endif //ANT_CryptoFunctions_H