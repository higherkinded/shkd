#pragma once

#include <cstdint>
#include <map>

#include <X11/X.h>
#include <X11/Xlib.h>

#include "constants.hh"
#include "keyboard/dsl.hh"
#include "types.hh"

namespace intkb {
    using keycode = KeyCode;
    using keyevent = XKeyEvent;

    struct key {
        keycode kc;
        uint8_t mask;

        key() noexcept: kc(0), mask(0) {}
        key(keyevent &ev): kc(ev.keycode), mask(ev.state) {}
        key(keycode kc, uint8_t mask = 0): kc(kc), mask(mask) {}

        auto operator<(const key &k)  const noexcept -> bool;
        auto operator==(const key &k) const noexcept -> bool;
    };

    class kstate {
        private:
            struct knode {
                std::map<key, knode *> keys{};
                char *const *command = nullptr;

                knode() = default;
            };

            Display *display;
            XEvent  event;
            Window  rootwin;

            knode *root = new knode;
            knode *current = root;

            inline auto allow_event(const XEvent &ev) -> bool;

            void push(const key &k) noexcept;

            void reset() noexcept;
            inline void exec()  noexcept;

        public:
            kstate(
                Display *display,
                const ty::array<dsl::kgrp> &sequences
            ) noexcept;

            auto next() noexcept -> bool;
    };
} // namespace intkb
