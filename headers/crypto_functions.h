
#ifndef ANT_CryptoFunctions_H
#define ANT_CryptoFunctions_H

#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1

#include <filesystem>
#include <optional>
#include <vector>

#include <cryptopp/aes.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/files.h>
#include <cryptopp/filters.h>
#include <cryptopp/md5.h>
#include <cryptopp/modes.h>

std::optional<std::array<std::uint64_t, 2>> Md5FromFile(const std::string &file);

bool EncryptAES(const std::array<std::byte, CryptoPP::AES::DEFAULT_KEYLENGTH> &key,
                const std::array<std::byte, CryptoPP::AES::BLOCKSIZE> &iv,
                const std::string &filename_in, const std::string &filename_out);

bool DecryptAES(const std::array<std::byte, CryptoPP::AES::DEFAULT_KEYLENGTH> &key,
                const std::array<std::byte, CryptoPP::AES::BLOCKSIZE> &iv,
                const std::string &filename_in, const std::string &filename_out);

#endif //ANT_CryptoFunctions_H