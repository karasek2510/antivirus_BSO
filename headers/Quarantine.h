//
// Created by karasek on 3/24/22.
//

#ifndef ANTIVIRUS_QUARANTINE_H
#define ANTIVIRUS_QUARANTINE_H


#include <vector>
#include <string>

std::string vectorToHexString(const std::vector<std::byte>& vector);

bool doQuarantine(const std::filesystem::path& path);

bool generateInfoFile(const std::filesystem::path& infoFilePath, const std::string& filename, const std::filesystem::path& originalLocation,
                      const std::vector<std::byte>& key, const std::vector<std::byte>& iv);

bool restoreFromQuarantine(const std::filesystem::path& filename);

#endif //ANTIVIRUS_QUARANTINE_H
