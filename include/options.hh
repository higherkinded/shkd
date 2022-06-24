#pragma once

struct options {
    bool daemonize = true;
    bool dry_run   = false;
    bool help      = false;
    bool loud      = true;

    options(int argc, char **argv) noexcept;
};

struct option {
    const char *short_switch;
    const char *long_switch;
    const char *description;
};

namespace o {
    constexpr option no_daemon
    { "-n", "--no-daemon", "Stay attached to the invoking terminal." };

    constexpr option silent
    { "-s", "--silent",    "Shut up and close the output descriptors." };

    constexpr option help
    { "-h", "--help",      "Show this message." };

    constexpr option all[] = { no_daemon, silent, help };
}
