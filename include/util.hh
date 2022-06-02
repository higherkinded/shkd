#pragma once

#include <string>

namespace util {
    void banner() noexcept;
    void die(int exitcode, const std::string &reason = "") noexcept;
    void warn(const std::string &message) noexcept;
    void help() noexcept;
};
