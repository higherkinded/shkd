#include <cstring>

#include "constants.hh"
#include "options.hh"
#include "util.hh"

static auto match(const char *arg, const option &o) -> bool {
    return !strcmp(arg, o.short_switch) || !strcmp(arg, o.long_switch);
}

options::options(int argc, char **argv) noexcept {
    if (argc > 64) util::die(
        exit_code::OPTIONS_ERR,
        "Fuzzing a hotkey daemon's argv earns you a place in Santa's naughty "
        "list. You should reconsider the decision you're making before getting "
        "a lump of coal in your Christmas sock."
    );

    for (int i = 1; i < argc; i++) {
        auto arg = argv[i];

        if (match(arg, o::no_daemon)) {
            daemonize = false;
            continue;
        }

        if (match(arg, o::help)) {
            help = true;
            dry_run = true;
            continue;
        }

        if (match(arg, o::silent)) {
            loud = false;
            continue;
        }

        util::die(exit_code::OPTIONS_ERR, "Unknown argument: '%s'", arg);
    }
}
