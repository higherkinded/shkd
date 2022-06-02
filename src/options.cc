#include <set>
#include <stdexcept>
#include <vector>

#include "options.hh"

options::options(int argc, char **argv) {
    if (argc > 64) throw std::runtime_error(
        "Fuzzing a hotkey daemon's argv earns you a place in Santa's naughty "
        "list. You should reconsider the decision you're making before getting "
        "a lump of coal in your Christmas sock."
    );

    const std::vector<std::string> args(argv + 1, argv + argc);
    std::set<std::string> encountered;

    for (const auto &arg : args) {
        if (encountered.contains(arg)) {
            throw std::runtime_error("Duplicate argument: '" + arg + "'");
        }

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

        throw std::runtime_error("Unknown argument: '" + arg + "'");
    }
}
