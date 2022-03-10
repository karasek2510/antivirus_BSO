#include <openssl/md5.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <optional>

#include "MD5.h"


std::vector<unsigned char> readBinaryFile(const std::string& path)
{
    std::vector<unsigned char> output;
    std::ifstream file(path, std::ios::binary);
    if (!file)
        std::cerr << "File not found!\n";
    else
        output.assign((std::istreambuf_iterator<char>(file)),std::istreambuf_iterator<char>());
    return output;
}

std::optional<std::string> md5FromFile(const std::string& filename)
{
    MD5_CTX context;
    if(!MD5_Init(&context))
    {
        return std::nullopt;
    }
    char buf[K_READ_BUF_SIZE];
    std::ifstream file(filename, std::ifstream::binary);
    while (file.good())
    {
        file.read(buf, sizeof(buf));
        if(!MD5_Update(&context, buf, file.gcount()))
        {
            return std::nullopt;
        }
    }
    unsigned char result[MD5_DIGEST_LENGTH];
    if(!MD5_Final(result, &context))
    {
        return std::nullopt;
    }
    std::stringstream shastr;
    shastr << std::hex << std::setfill('0');
    for (const auto &byte: result)
    {
        shastr << std::setw(2) << (int)byte;
    }
    return shastr.str();
}