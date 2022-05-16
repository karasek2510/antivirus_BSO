
#ifndef ANTIVIRUS_MONITOR_H
#define ANTIVIRUS_MONITOR_H

#include <future>

template<typename T>
bool FutureIsReady(std::future<T>* t);

bool IsFileInThreadsMap(const std::filesystem::path& path);

std::string EventToPath(struct inotify_event* event);

bool NotifyChanges(int fileDescriptor);

void NotifyChangesThread(int fileDescriptor);

void UserInputThread();

void ThreadWatcher();

bool MonitorDirectoryRecursively(const std::filesystem::path& path);

#endif //ANTIVIRUS_MONITOR_H
