#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "options.hh"
#include "util.hh"

void util::banner() {
    std::fprintf(stderr, "%s v%s\n", EXENAME, VERSION);
}

void util::warn(const char *fmt, ...) {
    va_list args;

    std::fprintf(stderr, "%s [warn]", EXENAME);

    va_start(args, fmt);
    std::vfprintf(stderr, fmt, args);
    va_end(args);

    std::fputc('\n', stderr);
}

void util::die(int exitcode, const char *fmt, ...) {
    va_list args;

    std::fprintf(stderr, "%s [die(%d)]", EXENAME, exitcode);

    va_start(args, fmt);
    std::vfprintf(stderr, fmt, args);
    va_end(args);

    std::fputc('\n', stderr);

    std::exit(exitcode);
}

void util::help() {
    std::fprintf(stderr, "Usage: %s [OPTIONS]\n", EXENAME);

    for(const auto & opt : o::all) {
        std::fprintf(stderr, "  %s %-11s %s\n",
                opt.short_switch, opt.long_switch, opt.description);
    }
}
