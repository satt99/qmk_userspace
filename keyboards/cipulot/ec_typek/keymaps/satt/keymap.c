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

// Special keycodes
#define LTENT LT(1, KC_ENTER)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    // clang-format off
    [0] = LAYOUT(
        GUI_E,    KC_GRV,   KC_1,  KC_2,  KC_3,     KC_4,    KC_5,     KC_6,    KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,   KC_BSLS,   CTALDEL,
        KC_PGUP,  KC_ESC,   KC_Q,  KC_W,  KC_E,     KC_R,    KC_T,              KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,   KC_DEL,
        KC_PGDN,  TAB_CTL,  KC_A,  KC_S,  KC_D,     KC_F,    KC_G,              KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,  KC_BSPC,
        GUI_R,    KC_LSFT,  KC_Z,  KC_X,  KC_C,     KC_V,    KC_B,              ZKHK,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,  KC_RSFT,   KC_RSFT,
                  KC_LGUI,                KC_LALT,  KC_SPC,  KC_SPC,            LTENT,    LTENT,    KC_RCTL,                                           MO(2)
    ),
    [1] = LAYOUT(
        CTLPGUP,  _______,  KC_F1,    KC_F2,    KC_F3,     KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,    KC_F12,   _______,  _______,
        ALTPGUP,  _______,  KC_1,     KC_2,     KC_3,      KC_4,     KC_5,               KC_6,     KC_7,     KC_8,     KC_9,     KC_0,      _______,  _______,  _______,
        ALTPGDN,  _______,  KC_F1,    KC_F2,    KC_F3,     KC_F4,    KC_F5,              KC_LEFT,  KC_DOWN,  KC_UP,    KC_RGHT,  KC_END,    _______,  _______,
        CTLPGDN,  _______,  KC_F6,    KC_F7,    KC_F8,     KC_F9,    KC_F10,             KC_HOME,  XXXXXXX,  KC_PGDN,  KC_PGUP,  XXXXXXX,   _______,  _______,  _______,
                  _______,                      _______,   _______,  _______,            _______,  _______,  _______,                                           _______
    ),
    [2] = LAYOUT(
        QK_BOOT,  UG_TOGG,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        KO_ON,    UG_NEXT,  _______,  _______,  _______,  _______,  _______,              _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        KO_OFF,   UG_PREV,  _______,  _______,  _______,  _______,  _______,              _______,  _______,  _______,  _______,  _______,  _______,  _______,
        KO_STAT,  _______,  _______,  _______,  _______,  _______,  _______,              _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
                  _______,                      _______,  _______,  _______,              _______,  _______,  _______,                                          _______)
};
