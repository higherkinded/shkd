#include <set>
#include <stdexcept>
#include <vector>

#include "constants.hh"
#include "options.hh"
#include "util.hh"

options::options(int argc, char **argv) noexcept {
    if (argc > 64) util::die(
        exit_code::OPTIONS_ERR,
        "Fuzzing a hotkey daemon's argv earns you a place in Santa's naughty "
        "list. You should reconsider the decision you're making before getting "
        "a lump of coal in your Christmas sock."
    );

    const std::vector<std::string> args(argv + 1, argv + argc);
    std::set<std::string> encountered;

    for (const auto &arg : args) {
        if (encountered.contains(arg)) util::die(
            exit_code::OPTIONS_ERR,
            "Duplicate argument: '" + arg + "'"
        );

        if (arg == "-n" || arg == "--no-daemon") {
            daemonize = false;
            encountered.insert("-n");
            encountered.insert("--no-daemon");
            continue;
        }

        if (arg == "-h" || arg == "--help") {
            help = true;
            dry_run = true;
            encountered.insert("-h");
            encountered.insert("--help");
            continue;
        }

        if (arg == "-s" || arg == "--silent") {
            loud = false;
            encountered.insert("-s");
            encountered.insert("--silent");
            continue;
        }

        util::die(exit_code::OPTIONS_ERR, "Unknown argument: '" + arg + "'");
    }
}
