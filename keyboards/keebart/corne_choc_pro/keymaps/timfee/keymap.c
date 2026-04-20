#include "timfee.h"
#include "timfee.c"

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [0] = LAYOUT_split_3x6_3(
        ESC_L2,  KC_Q,  KC_W,  KC_E,    KC_R,    KC_T,        KC_Y,    KC_U,    KC_I,     KC_O,    KC_P,    MIN_L1,
        KC_TAB,  KC_A,  KC_S,  KC_D,    KC_F,    KC_G,        KC_H,    KC_J,    KC_K,     KC_L,    KC_SCLN, KC_EQL,
        KC_LSFT, KC_Z,  KC_X,  KC_C,    KC_V,    KC_B,        KC_N,    KC_M,    KC_COMM,  KC_DOT,  KC_SLSH, KC_QUOT,
                                CT_GRV,  AL_DEL,  GU_BSP,      GU_SPC,  AL_ENT,  CT_BSL
    ),

    [1] = LAYOUT_split_3x6_3(
        _______, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC,     KC_7,    KC_8,   KC_9,    _______, _______, _______,
        _______, KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN,     KC_4,    KC_5,   KC_6,    _______, _______, _______,
        _______, _______, _______, _______, _______, _______,      KC_1,    KC_2,   KC_3,    _______, _______, _______,
                                   _______, _______, _______,      KC_COMM, KC_0,   KC_DOT
    ),

    [2] = LAYOUT_split_3x6_3(
        _______, KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,       LALT(KC_LEFT), LALT(KC_DOWN), LALT(KC_UP),   LALT(KC_RGHT), _______, RGB_MOD,
        _______, KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,      KC_LEFT,       KC_DOWN,       KC_UP,         KC_RGHT,       _______, RGB_RMOD,
        _______, _______, _______, _______, _______, _______,      RGUI(KC_LEFT), RGUI(KC_DOWN), RGUI(KC_UP),   RGUI(KC_RGHT), _______, RGB_TOG,
                                   QK_BOOT, _______, _______,      _______,       _______,       _______
    ),
};
