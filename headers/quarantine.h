
#ifndef ANTIVIRUS_QUARANTINE_H
#define ANTIVIRUS_QUARANTINE_H

#include <filesystem>
#include <string>
#include <vector>


std::filesystem::path GetFullPathQuarantine(const std::filesystem::path &file, const std::filesystem::path &directory);

template<std::size_t SIZE_KEY,std::size_t SIZE_IV>
bool GenerateInfoFile(const std::filesystem::path &infoFilePath, const std::string &filename,
                      const std::filesystem::path &originalLocation,
                      std::filesystem::perms perms, const std::array<std::byte, SIZE_KEY>  &key,
                      const std::array<std::byte, SIZE_IV>  &iv);

bool DoQuarantine(const std::filesystem::path &path);

bool RestoreFromQuarantine(const std::filesystem::path &filename);

void ShowFilesInQuarantine();

bool AlterQuarantinePermissions(int perms);


#endif //ANTIVIRUS_QUARANTINE_H
