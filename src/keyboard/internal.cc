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
    try {
        auto cmd = new std::vector<const char *>;
        for (const auto &arg : grp.command) cmd->push_back(arg.c_str());
        cmd->push_back(nullptr);

        std::vector<key> keys;
        for (auto &key : grp.keys) keys.push_back({
                XKeysymToKeycode(dsp, key.ks),
                key.mask,
                });

        sequence = keys;
        command = (char *const *) cmd->data();
    } catch (std::exception &) {
        util::die(41, "Failed to allocate a command buffer");
    }
}

kstate::kstate(
    Display *display,
    const std::vector<kseq> &sequences
) noexcept: display(display), rootwin(XDefaultRootWindow(display)) {
    if (sequences.empty()) util::die(10, "No sequences to handle");

    for (const auto &keys : sequences) {
        if (!keys.command || !keys.command[0]) continue;

        auto cur = root;

        for (const auto &k : keys.sequence) {
            if (!cur->keys.contains(k)) try {
                cur->keys.insert({k, new knode()});
            } catch (std::exception &) {
                util::die(42, "Cannot allocate a decision tree");
            }
            cur = cur->keys[k];
        }

        cur->command = keys.command;
    }

    reset();
}

auto kstate::knode::last() const noexcept -> bool {
    return !keys.size();
}

void kstate::reset() noexcept {
    if (&current != &root) ungrab();
    grab_root();
    in_root = true;
    current = root;
}

void kstate::ungrab() const noexcept {
    XUngrabKeyboard(display, CurrentTime);
    XUngrabKey(display, AnyKey, AnyModifier, rootwin);
}

void kstate::grab_root() const noexcept {
    for (const auto &[ k, _ ] : root->keys) {
        XGrabKey(display, k.kc, k.mask, rootwin, true, GrabModeAsync, GrabModeAsync);
    }
}

void kstate::grab_all() const noexcept {
    XGrabKeyboard(
        display, rootwin, true, GrabModeAsync, GrabModeAsync, CurrentTime);
}

void kstate::exec() noexcept {
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

    if (current->last()) return exec();

    grab_all();
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
