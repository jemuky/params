#pragma once

#include <mutex>

#include "base.h"

namespace params {

static std::mutex g_mtx_;

enum LogLevel {
    Debug = 0,
    Info = 1,
    Warn = 2,
    Error = 3,
};

static LogLevel g_log_lev_ = LogLevel::Debug;

static void set_log_lev(LogLevel lev) { g_log_lev_ = lev; }

//         前景    背景
// Default ESC[39m ESC[49m
// Black ESC[30m ESC[40m
// Dark red ESC[31m ESC[41m
// Dark green ESC[32m ESC[42m
// Dark yellow (Orange-ish) ESC[33m ESC[43m
// Dark blue ESC[34m ESC[44m
// Dark magenta ESC[35m ESC[45m
// Dark cyan ESC[36m ESC[46m
// Light gray ESC[37m ESC[47m
// Dark gray ESC[90m ESC[100m
// Red ESC[91m ESC[101m
// Green ESC[92m ESC[101m
// Orange ESC[93m ESC[103m
// Blue ESC[94m ESC[104m
// Magenta ESC[95m ESC[105m
// Cyan ESC[96m ESC[106m
// White ESC[97m ESC[107m
template <class... Args>
void __param_debug_print(const char* file, int line, const char* function, const char* color,
                         const char* prefix, const Args&... args) {
    std::lock_guard<std::mutex> lock(g_mtx_);
    std::cout << std::boolalpha << color << prefix << "(" << function << ") " << file << ":" << line
              << ": ";
    auto print_func = [](auto i) { std::cout << i; };
    std::initializer_list<int>{(print_func(args), 0)...};
    std::cout << "\u001b[39m" << std::endl;
}

#ifdef _WIN32
#define PARAM_FUNCTION __FUNCSIG__
#else
#define PARAM_FUNCTION __PRETTY_FUNCTION__
#endif

#define ERROR(...)                                                                        \
    if (static_cast<int>(g_log_lev_) <= static_cast<int>(LogLevel::Error)) {              \
        __param_debug_print(__FILE__, __LINE__, static_cast<const char*>(PARAM_FUNCTION), \
                            "\u001b[91m", "[ERROR] ", __VA_ARGS__);                       \
    }

#define WARN(...)                                                                         \
    if (static_cast<int>(g_log_lev_) <= static_cast<int>(LogLevel::Warn)) {               \
        __param_debug_print(__FILE__, __LINE__, static_cast<const char*>(PARAM_FUNCTION), \
                            "\u001b[33m", "[WARN] ", __VA_ARGS__);                        \
    }

#define INFO(...)                                                                         \
    if (static_cast<int>(g_log_lev_) <= static_cast<int>(LogLevel::Info)) {               \
        __param_debug_print(__FILE__, __LINE__, static_cast<const char*>(PARAM_FUNCTION), \
                            "\u001b[94m", "[INFO] ", __VA_ARGS__);                        \
    }

#define DEBUG(...)                                                                        \
    if (static_cast<int>(g_log_lev_) <= static_cast<int>(LogLevel::Debug)) {              \
        __param_debug_print(__FILE__, __LINE__, static_cast<const char*>(PARAM_FUNCTION), \
                            "\u001b[95m", "[DEBUG] ", __VA_ARGS__);                       \
    }

} // namespace params