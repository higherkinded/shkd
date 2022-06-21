#include <iostream>
#include <stdexcept>
#include <vector>

#include <unistd.h>
#include <sys/wait.h>

#include <X11/keysym.h>
#include <X11/XF86keysym.h>

#include "constants.hh"
#include "keyboard/internal.hh"
#include "options.hh"
#include "util.hh"

inline void handle_options(int argc, char **argv);
inline void chdir_home();
inline void install_sig_handlers();
inline auto open_display() -> Display *;

auto main(int argc, char **argv) noexcept -> int {
    chdir_home();
    auto display = open_display();
    handle_options(argc, argv);
    install_sig_handlers();

    using namespace dsl;
    std::vector<kgrp> keys {
        #include "keys"
    };

    std::vector<intkb::kseq> kseqs;
    for (const auto &grp : keys) kseqs.emplace_back(grp, display);

    intkb::kstate state(display, kseqs);
    for (;;) state.next();
}

inline void handle_options(int argc, char **argv) {
    options opts(argc, argv);
    if (opts.loud || opts.help) util::banner();
    if (opts.help) util::help();
    if (opts.dry_run) std::exit(0);
    if (opts.daemonize && daemon(true, opts.loud)) {
        util::die(exit_code::DAEMON_ERR, "Call to daemon() failed");
    }
}

inline void chdir_home() {
    const auto home = std::getenv("HOME");
    if (!home) util::die(exit_code::HOME_UNSET, "'HOME' variable isn't set");
    if (chdir(home)) util::die(
        exit_code::CANT_CHDIR, "Cannot chdir into 'HOME'");
}

void sigchld(int _) {
    while (0 < waitpid(-1, nullptr, WNOHANG));
}

inline void install_handler(int sig, void (&handler)(int)) {
    if (signal(sig, handler) == SIG_ERR) util::die(exit_code::SIGNAL_ERR,
        "Cannot install a signal handler for signal " + std::to_string(sig));
}

inline void install_sig_handlers() {
    install_handler(SIGCHLD, sigchld);
}

inline Display *open_display() {
    auto display = XOpenDisplay(std::getenv("DISPLAY"));
    if (!display) util::die(exit_code::DISPLAY_ERR, "Cannot open display");
    errno = 0;
    return display;
}
