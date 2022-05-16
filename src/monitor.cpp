
#include "../headers/monitor.h"
#include "../headers/thread_safe_queue.h"
#include "../headers/scanner.h"

#include <sys/inotify.h>

#include <filesystem>
#include <future>
#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>
#include <unordered_map>
#include <vector>


#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LEN (1024 * (EVENT_SIZE + 16))

int MAX_THREAD_N;

std::filesystem::path rootPath;
std::unordered_map<int, std::filesystem::path> watchDescriptorsMap;
SafeQueue<std::filesystem::path> pathQueue = SafeQueue<std::filesystem::path>();
std::map<std::future<bool> *, std::filesystem::path> threads{};


std::atomic_bool keepRunning = true;

template<typename T>
bool FutureIsReady(std::future<T> *t) { // checking if thread has been finished
    return t->wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}

bool IsFileInThreadsMap(const std::filesystem::path &path) { // checking if file has assigned thread
    for (auto &thread: threads) {
        if (thread.second == path) {
            return true;
        }
    }
    return false;
}

std::string EventToPath(struct inotify_event *event) {
    auto directory = watchDescriptorsMap.find(event->wd);
    std::string path = directory->second;
    if (path != rootPath) {
        path.append("/");
    }
    path.append(event->name);
    return path;
}

bool NotifyChanges(int fileDescriptor) {
    char buffer[BUF_LEN];
    long length;
    fd_set set;
    struct timeval timeout{};
    int rv;
    FD_ZERO(&set);
    FD_SET(fileDescriptor, &set);
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    rv = select(fileDescriptor + 1, &set, nullptr, nullptr, &timeout);
    if (rv <= 0) {
        return false;
    }
    length = read(fileDescriptor, buffer, BUF_LEN);
    int i = 0;
    if (length < 0) {
        std::cerr << "Cannot read events\n";
        return false;
    }
    while (i < length && keepRunning) {
        auto *event = (struct inotify_event *) &buffer[i];
        if (event->len) {
            if (event->mask & IN_ISDIR) {
                if (event->mask & IN_CREATE) { // new directory has been created
                    std::filesystem::path path = EventToPath(event);
                    int watchDescriptor = inotify_add_watch(fileDescriptor,
                                                            path.c_str(), // adding watch on new directory
                                                            IN_MODIFY | IN_CREATE | IN_DELETE);
                    watchDescriptorsMap[watchDescriptor] = path; // adding new descriptor to map
                } else if (event->mask & IN_DELETE) { // directory has been deleted
                    std::filesystem::path path = EventToPath(event);
                    // deleting descriptor from map
                    int directoryDescriptorToDelete = -1;
                    for (auto &it: watchDescriptorsMap) {
                        if (it.second == path) {
                            directoryDescriptorToDelete = it.first;
                        }
                    }
                    if (directoryDescriptorToDelete > 0) {
                        watchDescriptorsMap.erase(directoryDescriptorToDelete);
                    }
                }
            } else {
                if ((event->mask & IN_CREATE) || (event->mask & IN_MODIFY)) { //file modification or creation
                    std::filesystem::path path = EventToPath(event);
                    pathQueue.enqueue(path);
                }
            }
        }
        i += EVENT_SIZE + event->len;
    }
    return true;
}

// thread which allow stopping monitoring
void NotifyChangesThread(int fileDescriptor) {
    while (keepRunning) {
        NotifyChanges(fileDescriptor);
    }
}

// thread which allow stopping monitoring
void UserInputThread() {
    char c;
    while (true) {
        c = getchar();
        if (c == 'q' || c == 'Q') {
            keepRunning = false;
            std::cout << "Stopping monitoring...\n";
            pathQueue.enqueue("");
            break;
        }
    }
}

// thread which manage scanning threads
void ThreadWatcher() {
    while (keepRunning) {
        if (threads.size() < MAX_THREAD_N) { // checking if thread can be added
            std::filesystem::path path = pathQueue.dequeue();
            if (IsFileInThreadsMap(path)) {
                pathQueue.enqueue(path);
            } else {
                threads[new std::future{std::async(std::launch::async, ScanFile, path)}] = path;
                if (threads.size() == MAX_THREAD_N) {
                    std::cout << "All threads are being used...\n";
                }
            }
        }
        for (auto it = threads.cbegin(); it != threads.cend();) {
            if (FutureIsReady(it->first)) { // cleaning after thread has been finished
                it->first->wait();
                delete it->first;
                threads.erase(it++);
            } else {
                ++it;
            }
        }
    }
}

bool MonitorDirectoryRecursively(const std::filesystem::path &path) {
    std::cout << "Press \"q\" to stop monitoring module\n";
    std::vector<std::filesystem::path> directories;
    rootPath = path;
    directories.push_back(path);
    for (std::filesystem::path dir_entry:
            std::filesystem::recursive_directory_iterator(
                    path, std::filesystem::directory_options::skip_permission_denied)) {
        if (std::filesystem::is_directory(dir_entry)) {
            directories.push_back(dir_entry);
        }
    }
    int fileDescriptor = inotify_init();
    if (fileDescriptor < 0) {
        std::cerr << "Cannot start watcher\n";
        return false;
    }
    for (const std::filesystem::path &directory: directories) {
        int watchDescriptor = inotify_add_watch(fileDescriptor, directory.c_str(),
                                                IN_MODIFY | IN_CREATE | IN_DELETE);
        watchDescriptorsMap[watchDescriptor] = directory;
    }
    std::thread userInput(UserInputThread);
    std::thread watcher(ThreadWatcher);
    std::thread notifyChanges(NotifyChangesThread, fileDescriptor);
    notifyChanges.join();
    watcher.join();
    userInput.join();
    for (const auto &watchDescriptor: watchDescriptorsMap) {
        inotify_rm_watch(fileDescriptor, watchDescriptor.first);
    }
    for (auto it = threads.cbegin(); it != threads.cend();) {
        delete it->first;
        it++;
    }
    close(fileDescriptor);
    return true;
}
