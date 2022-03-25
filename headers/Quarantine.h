//
// Created by karasek on 3/24/22.
//

#ifndef ANTIVIRUS_QUARANTINE_H
#define ANTIVIRUS_QUARANTINE_H
void doQuarantine(std::filesystem::path path);

void generateInfoFile(std::filesystem::path infoFilePath,std::string filename,
                      std::filesystem::path originalLocation,std::vector<std::byte> key,
                      std::vector<std::byte> iv);

#endif //ANTIVIRUS_QUARANTINE_H
