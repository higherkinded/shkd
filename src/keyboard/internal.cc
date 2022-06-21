#include "keyboard/internal.hh"

#include <cstring>
#include <exception>
#include <set>
#include <sstream>

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

kseq::kseq(const dsl::kgrp &grp, Display *dsp) noexcept {
    // No exception handler. The program is so tiny that it won't matter. Ever.
    auto cmd = new std::vector<const char *>;
    for (const auto &arg : grp.command) cmd->push_back(arg.c_str());
    cmd->push_back(nullptr);

    std::vector<key> keys;
    for (auto &key : grp.keys) keys.emplace_back(
        XKeysymToKeycode(dsp, key.ks), key.mask);

    sequence = keys;
    command = (char *const *) cmd->data();
}

kstate::kstate(
    Display *display,
    const std::vector<kseq> &sequences
) noexcept: display(display), rootwin(XDefaultRootWindow(display)) {
    if (sequences.empty()) util::die(
        exit_code::NO_SEQUENCES, "No sequences to handle");

    for (const auto &keys : sequences) {
        if (!keys.command || !keys.command[0]) continue;

        auto cur = root;

        for (const auto &k : keys.sequence) {
            // No exception handler. shkd is virtually zero-cost.
            cur->keys.insert({k, new knode()});
            cur = cur->keys[k];
        }

        cur->command = keys.command;
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
    in_root = true;
    current = root;
}

inline void kstate::exec() noexcept {
    // Check command for at least one word and fork the execution parent
    if (current->command && current->command[0] && !fork()) {
        if (execvp(current->command[0], current->command)) {
            std::stringstream message;

            message << "Failed to run `";
            for (char *const *arg = current->command; arg;) {
                message << arg;
                if (++arg) message << ' ';
            }
            message << "` (" << strerror(errno) << ')';

            util::warn(message.str());
        }
    } else reset(); // Reset in parent process
}

void kstate::push(const key &k) noexcept {
    auto const &next = current->keys.find(k);
    if (next == current->keys.end()) return reset();

    current = next->second;
    in_root = false;

    if (!current->keys.size()) return exec();

    // Grab all
    XGrabKeyboard(
        display, rootwin, true, GrabModeAsync, GrabModeAsync, CurrentTime);
}

auto kstate::allow_event(const XEvent &ev) -> bool {
    static const std::set<KeySym> prevented {
        #include "preventkeys"
    };

    if (ev.type != KeyPress) return false;

    const auto &kev = ev.xkey;
    if (!current->keys.contains(kev.keycode)) return !prevented.contains(
        XkbKeycodeToKeysym(display, kev.keycode, 0, 0));

    return true;
}

auto kstate::next() noexcept -> bool {
    XNextEvent(display, &event);
    if (allow_event(event)) {
        push(event.xkey);
        return true;
    }
    return false;
}
