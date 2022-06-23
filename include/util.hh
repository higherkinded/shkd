#pragma once

namespace util {
    void banner();
    void die(int exitcode, const char *fmt, ...);
    void warn(const char *fmt, ...);
    void help();
};
