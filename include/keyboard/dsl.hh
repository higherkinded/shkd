#pragma once

#include <cstdint>

#include <X11/X.h>

#include "types.hh"

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

        k(keysym ks, uint8_t mask = 0): ks(ks), mask(mask) {}

        k(): k(0) {}
    };

    struct kgrp {
        ty::array<k> keys;
        ty::array<const char *> command;
    };
} // namespace dsl
