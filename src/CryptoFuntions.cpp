#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1 // required to hide compiler warning

#include "../headers/CryptoFuntions.h"

#include <iomanip>
#include <iostream>
#include <optional>

#include <cryptopp/aes.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/files.h>
#include <cryptopp/filters.h>
#include <cryptopp/md5.h>
#include <cryptopp/modes.h>

#include "../headers/DataManagement.h"

// computing hash of the file
std::optional<std::array<std::uint64_t, 2>> Md5FromFile(const std::string &file) {
    CryptoPP::Weak1::MD5 md5;
    std::array<byte, CryptoPP::Weak1::MD5::DIGESTSIZE> hashArray{};
    try {
        CryptoPP::FileSource fs(file.c_str(), true,
                                new CryptoPP::HashFilter(md5,
                                         new CryptoPP::ArraySink(hashArray.data(),
                                                         CryptoPP::Weak1::MD5::DIGESTSIZE)));
    }
    catch (CryptoPP::FileStore::Err &err) {
        return std::nullopt; // returned if hash can't be computed
    }
    std::array<std::uint64_t, 2> hash = ByteArray16ToUint64s(hashArray);
    return hash;
}

// encryption with AES CFB
bool EncryptAES(const std::array<std::byte, CryptoPP::AES::DEFAULT_KEYLENGTH> &key,
                const std::array<std::byte, CryptoPP::AES::BLOCKSIZE> &iv,
                const std::string &filename_in, const std::string &filename_out) {

    CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption cipher{};
    cipher.SetKeyWithIV(reinterpret_cast<const byte *>(key.data()), key.size(),
                        reinterpret_cast<const byte *>(iv.data()));

    std::ifstream in{filename_in, std::ios::binary};
    std::ofstream out{filename_out, std::ios::binary};
    try {
        CryptoPP::FileSource{in, true,
                             new CryptoPP::StreamTransformationFilter{
                                     cipher, new CryptoPP::FileSink{out}}};
    }catch (CryptoPP::FileSink::Err &err) {
        std::cerr << "Cannot encrypt file\n";
        return false; // returned if encryption can't be done
    }
    return true;

}
// decryption with AES CFB
bool DecryptAES(const std::array<std::byte, CryptoPP::AES::DEFAULT_KEYLENGTH> &key,
                const std::array<std::byte, CryptoPP::AES::BLOCKSIZE> &iv,
                const std::string &filename_in, const std::string &filename_out) {

    CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption cipher{};
    cipher.SetKeyWithIV(reinterpret_cast<const byte *>(key.data()), key.size(),
                        reinterpret_cast<const byte *>(iv.data()));

    std::ifstream in{filename_in, std::ios::binary};
    std::ofstream out{filename_out, std::ios::binary};
    try{
        CryptoPP::FileSource{in, true,
                             new CryptoPP::StreamTransformationFilter{
                                     cipher, new CryptoPP::FileSink{out}}};
    }catch (CryptoPP::FileSink::Err &err) {
        std::cerr << "Cannot decrypt file\n";
        return false; // returned if decryption can't be done
    }
    return true;
}
