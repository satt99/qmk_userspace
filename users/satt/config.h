#pragma once

// Tap-hold options must be visible to QMK's core, not only to the keymap.
#define HOLD_ON_OTHER_KEY_PRESS
#undef QUICK_TAP_TERM
#define QUICK_TAP_TERM 100
