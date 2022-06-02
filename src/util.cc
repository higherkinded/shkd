#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "options.hh"
#include "util.hh"

void util::banner() noexcept {
    std::cerr << EXENAME << " v" << VERSION << std::endl;
}

void util::warn(const std::string &message) noexcept {
    std::cerr << EXENAME << " [log] " << message << std::endl;
}

void util::die(int exitcode, const std::string &reason) noexcept {
    std::cerr << EXENAME << " [die(" << exitcode << ")]";

    if (!!reason.size()) std::cerr << " " << reason;
    if (errno) std::cerr << " " << "(err: " << strerror(errno) << ")";

    std::cerr << std::endl;
    std::exit(exitcode);
}

void util::help() noexcept {
    std::fprintf(stderr, "Usage: %s [OPTIONS]\n", EXENAME);

    for (int i = 0; i < o::count; i++) {
        const option &opt = o::all[i];
        std::fprintf(stderr, "  %s %-11s %s\n",
            opt.short_switch, opt.long_switch, opt.description);
    }
}
