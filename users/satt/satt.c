#include QMK_KEYBOARD_H
#include "eeconfig.h"
#include "keymap_japanese.h"
#include "process_key_override.h"
#include "satt.h"

user_config_t user_config = {};

void eeconfig_init_user(void) {
    user_config.raw          = 0; // Clear all bits first
    user_config.key_override = 1; // Default to override on
    eeconfig_update_user(user_config.raw);
}

void keyboard_post_init_user(void) {
    user_config.raw = eeconfig_read_user();
    // Initialize the key override state based on the user config
    if (user_config.key_override) {
        key_override_on();
    } else {
        key_override_off();
    }
}

void set_key_override(void) {
    // Early return to avoid unnecessary EEPROM writes if the state hasn't changed
    bool current_override = key_override_is_enabled();
    if (user_config.key_override == current_override) {
        return;
    }
    user_config.key_override = current_override;
    eeconfig_update_user(user_config.raw);
}

// This weak function allows users to override the default key processing behavior in their own keymap files.
// It is called by process_record_user for any keycode not handled directly, and should return true to allow further processing or false to block it.
__attribute__((weak)) bool process_record_keymap(uint16_t keycode, keyrecord_t *record) {
    return true;
}
static bool handle_key_override(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case QK_KEY_OVERRIDE_TOGGLE:
        case QK_KEY_OVERRIDE_ON:
        case QK_KEY_OVERRIDE_OFF:
            // Handle override and then update the user config
            if (record->event.pressed) {
                process_key_override(keycode, record);
                set_key_override();
            }
            return true;
        default:
            return false;
    }
}

static bool handle_zkhk_input(uint16_t keycode, keyrecord_t *record) {
    if (keycode != ZKHK) return false;
    if (record->event.pressed) {
        switch (detected_host_os()) {
            case OS_MACOS:
            case OS_IOS:
                if (get_mods() & MOD_MASK_CTRL) {
                    tap_code(KC_SPC);
                } else {
                    register_mods(MOD_MASK_CTRL);
                    tap_code(KC_SPC);
                    unregister_mods(MOD_MASK_CTRL);
                }
                break;
            default:
                if (user_config.key_override) {
                    tap_code(JP_ZKHK);
                } else if (get_mods() & MOD_MASK_ALT) {
                    tap_code(KC_GRV);
                } else {
                    register_mods(MOD_MASK_ALT);
                    tap_code(KC_GRV);
                    unregister_mods(MOD_MASK_ALT);
                }
        }
    }
    return true;
}

static bool handle_ko_stat(uint16_t keycode, keyrecord_t *record) {
    if (keycode != KO_STAT) return false;
    if (record->event.pressed) {
        send_string(key_override_is_enabled() ? "ko-on" : "ko-off");
    }
    return true;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (handle_key_override(keycode, record)) {
        return false;
    }
    if (handle_zkhk_input(keycode, record)) {
        return false;
    }
    if (handle_ko_stat(keycode, record)) {
        return false;
    }
    return process_record_keymap(keycode, record);
}
