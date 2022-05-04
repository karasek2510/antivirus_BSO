
#include <string>
#include <vector>
#include <filesystem>

#include <sys/inotify.h>
#include <unordered_map>
#include <iostream>
#include <unistd.h>
#include <thread>
#include <future>

#include "../headers/monitor.h"
#include "../headers/thread_safe_queue.h"
#include "../headers/scanner.h"

#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LEN (1024 * (EVENT_SIZE + 16))

int MAX_THREAD_N;

std::unordered_map<int, std::filesystem::path> watchDescriptorsMap;

SafeQueue<std::filesystem::path> pathQueue = SafeQueue<std::filesystem::path>();
std::map<std::future<bool>*, std::filesystem::path> threads {};

template<typename T>
bool futureIsReady(std::future<T>* t){
    return t->wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}

bool isFileInThreadsMap(std::filesystem::path path){
    for (auto it = threads.begin(); it != threads.end(); ++it){
        if (it->second == path){
            return true;
        }
    }
    return false;
}

void threadWatcher(){

    while(true){
        if(threads.size()<MAX_THREAD_N){
            std::filesystem::path path = pathQueue.dequeue();
            if(isFileInThreadsMap(path)){
               pathQueue.enqueue(path);
            }else{
                threads[new std::future{std::async(std::launch::async, ScanFile, path)}] = path;
                std::cout << "Threads in vector: " << threads.size() << "\n";
            }
        }
        for (auto it = threads.cbegin(); it != threads.cend();){
            if(futureIsReady(it->first)){
                threads.erase(it++);
                std::cout << "Thread has been removed!!!\n";
            }else{
                ++it;
            }

        }
    }
}

std::string eventToPath(struct inotify_event* event) {
    auto directory = watchDescriptorsMap.find (event->wd);
    std::string path= directory->second;
    path.append("/");
    path.append(event->name);
    return path;
}

bool notifyChanges(int fileDescriptor){
    char buffer[BUF_LEN];
    int length = read(fileDescriptor, buffer,BUF_LEN);
    int i = 0;
    if ( length < 0 ) {
        std::cerr << "Cannot read events\n";
        return false;
    }
    while ( i < length ) {
        struct inotify_event *event = (struct inotify_event*) &buffer[i];
        if(event->len){
            if(event->mask & IN_ISDIR){
                if(event->mask & IN_CREATE){
                    std::filesystem::path path = eventToPath(event);
                    int watchDescriptor = inotify_add_watch(fileDescriptor,path.c_str(),
                                                           IN_MODIFY | IN_CREATE | IN_DELETE);
                    watchDescriptorsMap[watchDescriptor] = path;
                    std::cout << "New directory: " << path.string() << "\n";
                }else if (event->mask & IN_MODIFY){
                    std::filesystem::path path = eventToPath(event);
                    std::cout << "Modified directory: " << path.string() << "\n";
                }else if (event->mask & IN_DELETE){
                    std::filesystem::path path = eventToPath(event);
                    int directoryDescriptorToDelete = -1;
                    for (std::unordered_map<int,std::filesystem::path>::const_iterator it = watchDescriptorsMap.begin();
                            it != watchDescriptorsMap.end(); ++it) {
                        if (it->second == path){
                            directoryDescriptorToDelete = it->first;
                        }
                    }
                    if(directoryDescriptorToDelete>0){
                        watchDescriptorsMap.erase(directoryDescriptorToDelete);
                    }
                    std::cout << "Deleted directory: " << path.string() << "\n";
                }
            }else{
                if(event->mask & IN_CREATE){
                    std::filesystem::path path = eventToPath(event);
                    pathQueue.enqueue(path);
                    std::cout << "Created file: " << path.string() << "\n";
                }else if (event->mask & IN_MODIFY){
                    std::filesystem::path path = eventToPath(event);
                    pathQueue.enqueue(path);
                    std::cout << "Modified file: " << path.string() << "\n";
                }else if (event->mask & IN_DELETE){
                    std::filesystem::path path = eventToPath(event);
                    std::cout << "Deleted file: " << path.string() << "\n";
                }
            }
        }
        i+= EVENT_SIZE + event->len;
    }
}

bool monitorDirectoryRecursively(const std::filesystem::path& path){
    std::vector<std::filesystem::path> directories;
    directories.push_back(path);
    for (std::filesystem::path dir_entry :
            std::filesystem::recursive_directory_iterator(path,std::filesystem::directory_options::skip_permission_denied))
    {
        if(std::filesystem::is_directory(dir_entry)){
            directories.push_back(dir_entry);
        }
    }
    int fileDescriptor = inotify_init();
    if (fileDescriptor < 0 ) {
        std::cerr << "Cannot start watcher\n";
        return false;
    }
    for(std::filesystem::path directory : directories){
        int watchDescriptor = inotify_add_watch(fileDescriptor, directory.c_str(),
                                                IN_MODIFY | IN_CREATE | IN_DELETE);
        watchDescriptorsMap[watchDescriptor] = directory;
    }
    std::thread watcher (threadWatcher);
    while(true){
        notifyChanges(fileDescriptor);
    }
    for (const auto& watchDescriptor : watchDescriptorsMap){
        inotify_rm_watch(fileDescriptor,watchDescriptor.first);
    }
    close(fileDescriptor);

}
