#pragma once

enum exit_code {
    HOME_UNSET   = 1,
    CANT_CHDIR   = 2,
    DISPLAY_ERR  = 3,
    OPTIONS_ERR  = 4,
    DAEMON_ERR   = 5,
    NO_SEQUENCES = 10,
    SIGNAL_ERR   = 90,
};
