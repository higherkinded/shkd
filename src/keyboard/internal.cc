#include "keyboard/internal.hh"

#include <cerrno>
#include <cstdio>
#include <cstring>

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/XKBlib.h>
#include <unistd.h>

#include "keyboard/dsl.hh"
#include "util.hh"

using namespace intkb;

auto key::operator<(const key &k) const noexcept -> bool {
    if (kc == k.kc) return mask < k.mask;
    return kc < k.kc;
}

auto key::operator==(const key &k) const noexcept -> bool {
    return (kc == k.kc) && (mask == k.mask);
}

kstate::kstate(
    Display *display,
    const ty::array<dsl::kgrp> &sequences
) noexcept: display(display), rootwin(XDefaultRootWindow(display)) {
    if (sequences.len == 0) util::die(
        exit_code::NO_SEQUENCES, "No sequences to handle");

    for (int seqid = 0; seqid < sequences.len; seqid++) {
        auto &sequence = sequences.data[seqid];

        if (!sequence.command.len) continue;

        auto cur = root;

        for (int kid = 0; kid < sequence.keys.len; kid++) {
            auto &symk = sequence.keys.data[kid];
            key k{ XKeysymToKeycode(display, symk.ks), symk.mask };

            // No exception handler. shkd is virtually zero-cost.
            cur->keys.insert({ k, new knode() });
            cur = cur->keys[k];
        }

        cur->command = (char *const *) sequence.command.data;
    }

    reset();
}

void kstate::reset() noexcept {
    if (&current != &root) { // Ungrab all
        XUngrabKeyboard(display, CurrentTime);
        XUngrabKey(display, AnyKey, AnyModifier, rootwin);
    }

    // Grab root
    for (const auto &[ k, _ ] : root->keys) XGrabKey(
        display, k.kc, k.mask, rootwin, true, GrabModeAsync, GrabModeAsync);
    current = root;
}

inline void kstate::exec() noexcept {
    // Check command for at least one word and fork the execution parent
    if (current->command && current->command[0] && !fork()) {
        if (execvp(current->command[0], current->command)) {
            util::warn("Failed to run command:");
            std::fputc('`', stderr);

            for (int i = 0; current->command[i]; i++) {
                std::fprintf(stderr, " %s", (char *) current->command[i]);
            }

            std::fprintf(stderr, "`: %s\n", strerror(errno));
        }
    } else reset(); // Reset in parent process
}

void kstate::push(const key &k) noexcept {
    auto const &next = current->keys.find(k);
    if (next == current->keys.end()) return reset();

    current = next->second;
    if (!current->keys.size()) return exec();

    // Grab all
    XGrabKeyboard(
        display, rootwin, true, GrabModeAsync, GrabModeAsync, CurrentTime);
}

inline auto kstate::allow_event(const XEvent &ev) -> bool {
    static const KeySym prevented[] {
        #include "preventkeys"
        0
    };

    if (ev.type != KeyPress) return false;

    const auto &kev = ev.xkey;
    if (!current->keys.contains(kev.keycode)) {
        auto ks = XkbKeycodeToKeysym(display, kev.keycode, 0, 0);
        for (int i = 0; prevented[i]; i++) if (prevented[i] == ks) return false;
    }

    return true;
}

auto kstate::next() noexcept -> bool {
    XNextEvent(display, &event);
    if (allow_event(event)) {
        push(event.xkey);
        return true;
    } else {
        printf("Disallowed event\n");
    }
    return false;
}
