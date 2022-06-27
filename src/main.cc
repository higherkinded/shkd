#include <cerrno>

#include <unistd.h>
#include <sys/wait.h>

#include <X11/keysym.h>
#include <X11/XF86keysym.h>
#include <X11/Xlib.h>

#include "constants.hh"
#include "keyboard/internal.hh"
#include "options.hh"
#include "types.hh"
#include "util.hh"

inline void handle_options(int argc, char **argv);
inline void chdir_home();
inline void install_sig_handlers();
inline auto open_display() -> Display *;

auto main(int argc, char **argv) noexcept -> int {
    XInitThreads();

    chdir_home();
    auto display = open_display();
    handle_options(argc, argv);
    install_sig_handlers();

    using namespace dsl;
    ty::array<kgrp> keys {
        #include "keys"
    };

    intkb::kstate state(display, keys);
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
        "Cannot install a signal handler for signal %d", sig);
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
