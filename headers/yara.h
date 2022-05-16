
#ifndef ANTIVIRUS_YARA_H
#define ANTIVIRUS_YARA_H

bool InitializeYaraDetector(const std::vector<std::filesystem::path> &rule_paths);

bool ScanUsingYaraDetector(const std::filesystem::path &path);

#endif //ANTIVIRUS_YARA_H
