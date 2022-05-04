
#ifndef ANTIVIRUS_MONITOR_H
#define ANTIVIRUS_MONITOR_H

std::string eventToPath(struct inotify_event* event);

bool notifyChanges(int fileDescriptor);

bool monitorDirectoryRecursively(const std::filesystem::path& path);

#endif //ANTIVIRUS_MONITOR_H
