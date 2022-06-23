#pragma once

#include <cstdint>
#include <vector>

#include <X11/X.h>

namespace dsl {
    using keysym = KeySym;

    enum mod {
        Shift = ShiftMask,
        Ctrl  = ControlMask,
        Mod1  = Mod1Mask,
        Mod2  = Mod2Mask,
        Mod3  = Mod3Mask,
        Mod4  = Mod4Mask,
        Mod5  = Mod5Mask,
    };

    struct kgrp;

    struct k {
        keysym  ks;
        uint8_t mask;

        constexpr k(keysym ks, uint8_t mask = 0): ks(ks), mask(mask) {}
    };

    struct kgrp {
        std::vector<k> keys;
        std::vector<const char *> command;
    };
} // namespace dsl
