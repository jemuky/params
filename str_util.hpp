#pragma once

#include <sstream>
#include <string>

#include "log.hpp"

namespace params {
static std::string trim(const std::string& str, char sep) {
    size_t first = str.find_first_not_of(sep);
    if (std::string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(sep);
    return str.substr(first, (last - first + 1));
}

static std::vector<std::string> split_space(const std::string& str) {
    std::istringstream iss(str);
    std::vector<std::string> tokens;

    // Iterate through each token in the stringstream
    std::string token;
    while (iss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}
static std::vector<std::string> split(const std::string& str, const std::string& sep) {
    size_t seplen = sep.size();

    std::vector<std::string> res;
    std::string newstr = str;
    size_t index;
    while (newstr.size() > 0) {
        index = newstr.find(sep);
        if (index != -1) {
            std::string sub = newstr.substr(0, index);

            if (!sub.empty()) {
                res.emplace_back(sub);
            }

            newstr = newstr.substr(index + seplen);
        } else {
            res.emplace_back(newstr);
            break;
        }
    }
    return res;
}

static std::pair<std::string, std::string> split_two_part(const std::string& str, char sep) {
    std::string prefix;
    std::string suffix;
    auto p = str.find(sep);
    if (p == std::string::npos) {
        return std::make_pair(str, "");
    }
    prefix = str.substr(0, p);
    suffix = str.substr(p + 1);
    return std::make_pair(prefix, suffix);
}

template <typename T>
static bool str_to_t(const std::string& str, T& t) {
    std::istringstream iss(str);
    return (iss >> t) ? true : false;
}
} // namespace params