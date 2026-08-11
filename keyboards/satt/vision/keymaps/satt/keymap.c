/*
Copyright 2024 SatT

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include QMK_KEYBOARD_H
#include "satt.h"

const uint16_t keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    // clang-format off

	[_QWERTY] = LAYOUT(
        GUI_E,   KC_ESC,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,             KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_DEL,  KC_DEL,
        GUI_R,   TAB_CTL, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,             KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_BSPC,
                 KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    ZKHK,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_QUOT,
                 KC_LGUI,                   KC_LALT, TL_LOWR, KC_SPC,  TL_UPPR, KC_ENT,  KC_RALT,                            CTALDEL
    ),

	[_LOWER] = LAYOUT(
        _______, _______, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC,          KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_GRV,  KC_TILD,
        _______, _______, KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,            KC_BSLS, KC_MINS, KC_EQL,  KC_LBRC, KC_RBRC, _______,
                 _______, KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  _______, KC_PIPE, KC_UNDS, KC_PLUS, KC_LCBR, KC_RCBR, _______,
                 _______,                   _______, _______, _______, _______, _______, _______,                            _______
    ),

	[_RAISE] = LAYOUT(
        _______, _______, KC_1,    KC_2,    KC_3,    KC_4,    KC_5,             KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    _______, _______,
        _______, _______, _______, _______, _______, _______, _______,          KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, KC_END,  _______,
                 _______, _______, _______, _______, _______, _______, KC_HOME, XXXXXXX, KC_PGDN, KC_PGUP, XXXXXXX, _______, _______,
                 _______,                   _______, _______, _______, _______, _______, _______,                            _______
    ),

	[_ADJUST] = LAYOUT(
        QK_BOOT, KO_ON,   KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,            KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,
        _______, KO_OFF,  _______, _______, _______, _______, _______,          _______, _______, _______, _______, _______, _______,
                 KO_STAT, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
                 _______,                   _______, _______, _______, _______, _______, _______,                            _______
    )
};
