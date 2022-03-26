//
// Created by karasek on 3/24/22.
//

#ifndef ANTIVIRUS_QUARANTINE_H
#define ANTIVIRUS_QUARANTINE_H

std::string vectorToHexString(std::vector<std::byte> vector);

void doQuarantine(std::filesystem::path path);

void generateInfoFile(std::string filename, std::filesystem::path originalLocation,std::vector<std::byte> key, std::vector<std::byte> iv);

void restoreFromQuarantine(std::filesystem::path filename);

#endif //ANTIVIRUS_QUARANTINE_H
