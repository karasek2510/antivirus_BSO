
#ifndef ANTIVIRUS_QUARANTINE_H
#define ANTIVIRUS_QUARANTINE_H

#include <string>
#include <vector>



template<std::size_t SIZE>
std::string arrayToHexString(const std::array<std::byte, SIZE> &array);

template<std::size_t SIZE>
std::array<std::byte, SIZE> hexStringToArray(const std::string &hex);

bool doQuarantine(const std::filesystem::path &path);

template<std::size_t SIZE_KEY,std::size_t SIZE_IV>
bool generateInfoFile(const std::filesystem::path &infoFilePath, const std::string &filename,
                      const std::filesystem::path &originalLocation,
                      std::filesystem::perms perms, const std::array<std::byte, SIZE_KEY>  &key,
                      const std::array<std::byte, SIZE_IV>  &iv);

bool restoreFromQuarantine(const std::filesystem::path &filename);

#endif //ANTIVIRUS_QUARANTINE_H
