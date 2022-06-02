#include <iostream>
#include <stdexcept>
#include <vector>

#include <X11/keysym.h>
#include <X11/XF86keysym.h>
#include <unistd.h>

#include "keyboard/internal.hh"
#include "options.hh"
#include "util.hh"

inline void handle_options(int argc, char **argv);
inline void chdir_home();
inline Display *open_display();

auto main(int argc, char **argv) noexcept -> int {
    chdir_home();
    auto display = open_display();
    handle_options(argc, argv);

    using namespace dsl;
    std::vector<kgrp> keys {
        #include "keys"
    };

    std::vector<intkb::kseq> kseqs;
    for (const auto &grp : keys) kseqs.push_back(intkb::kseq(grp, display));

    intkb::kstate state(display, kseqs);
    for (;;) state.next();
}

inline void handle_options(int argc, char **argv) {
    try {
        options opts(argc, argv);
        if (opts.loud || opts.help) util::banner();
        if (opts.help) util::help();
        if (opts.dry_run) std::exit(0);
        if (opts.daemonize && daemon(true, opts.loud)) {
            util::die(5, "Call to daemon() failed");
        }
    } catch (const std::runtime_error &e) {
        util::die(4, e.what());
    }
}

inline void chdir_home() {
    const auto home = std::getenv("HOME");
    if (!home) util::die(1, "'HOME' variable isn't set");
    if (chdir(home)) util::die(2, "Cannot chdir into 'HOME'");
}

inline Display *open_display() {
    auto display = XOpenDisplay(std::getenv("DISPLAY"));
    if (!display) util::die(3, "Cannot open display");
    errno = 0;
    return display;
}
