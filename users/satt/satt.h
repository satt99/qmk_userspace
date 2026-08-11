#ifndef USERSPACE
#define USERSPACE

// Tap hold configuration
#define HOLD_ON_OTHER_KEY_PRESS
#undef QUICK_TAP_TERM
#define QUICK_TAP_TERM 100

// Prototypes
bool process_record_keymap(uint16_t keycode, keyrecord_t *record);

// Layer names
enum layer_names {
    _QWERTY = 0,
    _LOWER,
    _RAISE,
    _ADJUST,
};

// Custom keycodes
enum custom_keycodes {
    ZKHK = QK_USER,
    KO_STAT,
};

// Special keycodes
#define TAB_CTL CTL_T(KC_TAB)
#define CTALDEL LCA(KC_DEL)
#define GUI_E LGUI(KC_E)
#define GUI_R LGUI(KC_R)
#define CTLPGUP LCTL(KC_PGUP)
#define CTLPGDN LCTL(KC_PGDN)
#define ALTPGUP LALT(KC_PGUP)
#define ALTPGDN LALT(KC_PGDN)

// User config
typedef union {
    uint32_t raw;
    struct {
        bool key_override : 1;
    };
} user_config_t;

#endif
