#include <openssl/md5.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <optional>

#include "../headers/MD5.h"

std::optional<std::string> md5FromFile(const std::string& filename)
{
    MD5_CTX context;
    if(!MD5_Init(&context))
    {
        return std::nullopt;
    }
    char buf[K_READ_BUF_SIZE];
    std::ifstream file(filename, std::ifstream::binary);
    if(file.good()){
        while(file.good())
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
        std::stringstream md5str;
        md5str << std::hex << std::setfill('0');
        for (const auto &byte: result)
        {
            md5str << std::setw(2) << (int)byte;
        }
        return md5str.str();
    }else{
        return std::nullopt;
    }
}