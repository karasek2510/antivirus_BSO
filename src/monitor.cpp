//
// Created by karasek on 4/27/22.
//

#include <string>
#include <vector>
#include <filesystem>

#include <sys/types.h>
#include <sys/inotify.h>

#include "../headers/monitor.h"

bool monitorDirectoryRecursively(const std::filesystem::path& path){
    std::vector<std::string> directories{};
    for (std::filesystem::path dir_entry :
            std::filesystem::recursive_directory_iterator(path,std::filesystem::directory_options::skip_permission_denied))
    {
        if(std::filesystem::is_directory(dir_entry)){
            directories.push_back(dir_entry);
        }
    }
    int fd = inotify_init();

}
