//
// Created by karasek on 3/10/22.
//

#ifndef ANT_MD5_H
#define ANT_MD5_H

#include <vector>
#include <optional>

static const int K_READ_BUF_SIZE{1024 * 16};

std::optional<std::string> md5FromFile(const std::string &file);

#endif //ANT_MD5_H