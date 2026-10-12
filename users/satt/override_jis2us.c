#include "keymap_japanese.h" // For JP_ keycode aliases

// Overrides to make a US layout keyboard work correctly when recognized as JIS by the OS
// Never resend the original trigger when a modifier change deactivates an override.
#define SATT_OVERRIDE_OPTIONS (ko_options_default | ko_option_no_reregister_trigger)
#define ko_make_satt(trigger_mods, trigger_key, replacement_key, negative_mods) \
    ko_make_with_layers_negmods_and_options(trigger_mods, trigger_key, replacement_key, ~0, negative_mods, SATT_OVERRIDE_OPTIONS)

// Keep the physically held Shift active when the JIS replacement needs Shift too.
// The replacement must be the unshifted base keycode, not an S(...) alias.
#define ko_make_shift_preserving(trigger_key, replacement_key) \
    ((const key_override_t){                                    \
        .trigger_mods      = MOD_MASK_SHIFT,                    \
        .layers            = ~0,                                \
        .suppressed_mods   = 0,                                 \
        .options           = SATT_OVERRIDE_OPTIONS,             \
        .negative_mod_mask = 0,                                 \
        .custom_action     = NULL,                              \
        .context           = NULL,                              \
        .trigger           = (trigger_key),                     \
        .replacement       = (replacement_key),                 \
        .enabled           = NULL                               \
    })

// clang-format off

// --- Unshifted keycode overrides ---
// These should only trigger when Shift is NOT held down.
// comments: (what OS sees without override) -> (what we want)
const key_override_t lbrc_override = ko_make_satt(0, KC_LBRC, JP_LBRC, MOD_MASK_SHIFT); // @ -> [
const key_override_t rbrc_override = ko_make_satt(0, KC_RBRC, JP_RBRC, MOD_MASK_SHIFT); // [ -> ]
const key_override_t quot_override = ko_make_satt(0, KC_QUOT, JP_QUOT, MOD_MASK_SHIFT); // : -> '
const key_override_t eql_override  = ko_make_satt(0, KC_EQL,  JP_EQL,  MOD_MASK_SHIFT); // ^ -> =
const key_override_t bsls_override = ko_make_satt(0, KC_BSLS, JP_YEN,  MOD_MASK_SHIFT); // ] -> ¥ (Since JP_BSLS does not work on macOS, we use JP_YEN instead)
const key_override_t grv_override  = ko_make_satt(0, KC_GRV,  JP_GRV,  MOD_MASK_SHIFT); // Zenkaku -> `

// --- Shifted keycode overrides ---
// comments: (what OS sees without override) -> (what we want)
const key_override_t sft_2_override    = ko_make_satt(MOD_MASK_SHIFT, KC_2,    JP_AT,   0); // " -> @
const key_override_t sft_6_override    = ko_make_satt(MOD_MASK_SHIFT, KC_6,    JP_CIRC, 0); // & -> ^
const key_override_t sft_7_override    = ko_make_shift_preserving(KC_7,    JP_6);    // ' -> &
const key_override_t sft_8_override    = ko_make_shift_preserving(KC_8,    JP_COLN); // ( -> *
const key_override_t sft_9_override    = ko_make_shift_preserving(KC_9,    JP_8);    // ) -> (
const key_override_t sft_0_override    = ko_make_shift_preserving(KC_0,    JP_9);    //   -> )
const key_override_t sft_mins_override = ko_make_shift_preserving(KC_MINS, JP_BSLS); // = -> _
const key_override_t sft_eql_override  = ko_make_shift_preserving(KC_EQL,  JP_SCLN); // ~ -> +
const key_override_t sft_lbrc_override = ko_make_shift_preserving(KC_LBRC, JP_LBRC); // ` -> {
const key_override_t sft_rbrc_override = ko_make_shift_preserving(KC_RBRC, JP_RBRC); // { -> }
const key_override_t sft_scln_override = ko_make_satt(MOD_MASK_SHIFT, KC_SCLN, JP_COLN, 0); // + -> :
const key_override_t sft_quot_override = ko_make_shift_preserving(KC_QUOT, JP_2);    // * -> "
const key_override_t sft_grv_override  = ko_make_shift_preserving(KC_GRV,  JP_CIRC); // Zenkaku -> ~
const key_override_t sft_bsls_override = ko_make_shift_preserving(KC_BSLS, JP_YEN);  // } -> |

// --- Shifted keycode alias overrides ---
// These trigger when an alias keycode is used
// comments: (what OS sees without override) -> (what we want)
const key_override_t ampr_override  = ko_make_satt(0, KC_AMPR, JP_AMPR, 0); // ' -> &
const key_override_t astr_override  = ko_make_satt(0, KC_ASTR, JP_ASTR, 0); // ( -> *
const key_override_t lprn_override  = ko_make_satt(0, KC_LPRN, JP_LPRN, 0); // ) -> (
const key_override_t rprn_override  = ko_make_satt(0, KC_RPRN, JP_RPRN, 0); //   -> )
const key_override_t unds_override  = ko_make_satt(0, KC_UNDS, JP_UNDS, 0); // = -> _
const key_override_t plus_override  = ko_make_satt(0, KC_PLUS, JP_PLUS, 0); // ~ -> +
const key_override_t lcbr_override  = ko_make_satt(0, KC_LCBR, JP_LCBR, 0); // ` -> {
const key_override_t rcbr_override  = ko_make_satt(0, KC_RCBR, JP_RCBR, 0); // { -> }
const key_override_t dquo_override  = ko_make_satt(0, KC_DQUO, JP_DQUO, 0); // * -> "
const key_override_t tild_override  = ko_make_satt(0, KC_TILD, JP_TILD, 0); // Zenkaku -> ~
const key_override_t pipe_override  = ko_make_satt(0, KC_PIPE, JP_PIPE, 0); // } -> |
// These require both shifted and non-shifted overrides to be defined
const key_override_t at_override    = ko_make_satt(0, KC_AT,   JP_AT,   MOD_MASK_SHIFT); // " -> @
const key_override_t circ_override  = ko_make_satt(0, KC_CIRC, JP_CIRC, MOD_MASK_SHIFT); // & -> ^
const key_override_t coln_override  = ko_make_satt(0, KC_COLN, JP_COLN, MOD_MASK_SHIFT); // + -> :
const key_override_t sft_at_override    = ko_make_satt(MOD_MASK_SHIFT, KC_AT,   JP_AT,   0); // " -> @
const key_override_t sft_circ_override  = ko_make_satt(MOD_MASK_SHIFT, KC_CIRC, JP_CIRC, 0); // & -> ^
const key_override_t sft_coln_override  = ko_make_satt(MOD_MASK_SHIFT, KC_COLN, JP_COLN, 0); // + -> :


// This globally defines all key overrides to be used
const key_override_t *const key_overrides[] = {
    // Unshifted
    &lbrc_override,
    &rbrc_override,
    &quot_override,
    &eql_override,
    &bsls_override,
    &grv_override,

    // Shifted
    &sft_2_override,
    &sft_6_override,
    &sft_7_override,
    &sft_8_override,
    &sft_9_override,
    &sft_0_override,
    &sft_mins_override,
    &sft_eql_override,
    &sft_lbrc_override,
    &sft_rbrc_override,
    &sft_scln_override,
    &sft_quot_override,
    &sft_grv_override,
    &sft_bsls_override,

    // Alias keycodes
    &ampr_override,
    &astr_override,
    &lprn_override,
    &rprn_override,
    &unds_override,
    &plus_override,
    &lcbr_override,
    &rcbr_override,
    &dquo_override,
    &tild_override,
    &pipe_override,
    &at_override,
    &circ_override,
    &coln_override,
    &sft_at_override,
    &sft_circ_override,
    &sft_coln_override,

    NULL, // Terminator
};
