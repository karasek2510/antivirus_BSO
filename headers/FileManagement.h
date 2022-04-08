
#ifndef ANTIVIRUS_BSO_FILEMANAGEMENT_H
#define ANTIVIRUS_BSO_FILEMANAGEMENT_H

#include <filesystem>
#include <unordered_set>

class HashArrayUint64_t {
public:
    size_t operator()(const std::array<std::uint64_t, 2> &arr) const {
        return arr[0];
    }
};

std::array<std::uint64_t, 2> stringHashToUint64s(const std::string& strHash);

bool getFileContent(std::unordered_set<std::array<std::uint64_t, 2>, HashArrayUint64_t> &uSet, const std::string &file);

bool isHashInUnorderedSet(std::unordered_set<std::array<std::uint64_t, 2>, HashArrayUint64_t> &uSet,
                          std::array<std::uint64_t, 2> hash);

int checkFileFs(const std::filesystem::path& path);

#endif //ANTIVIRUS_BSO_FILEMANAGEMENT_H