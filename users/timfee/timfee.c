#include "timfee.h"

// ── State for require-prior-idle ──
static uint16_t last_key_time = 0;
static uint16_t last_keycode  = KC_NO;

// ── Combos (matching Vial config) ──
const uint16_t PROGMEM lparen_combo[] = {KC_R, KC_T, COMBO_END};
const uint16_t PROGMEM rparen_combo[] = {KC_Y, KC_U, COMBO_END};
const uint16_t PROGMEM lbkt_combo[]   = {KC_F, KC_G, COMBO_END};
const uint16_t PROGMEM rbkt_combo[]   = {KC_H, KC_J, COMBO_END};
const uint16_t PROGMEM lbrace_combo[] = {KC_V, KC_B, COMBO_END};
const uint16_t PROGMEM rbrace_combo[] = {KC_N, KC_M, COMBO_END};
const uint16_t PROGMEM pipe_combo[]   = {Z_L1, KC_X, COMBO_END};
const uint16_t PROGMEM bslh_combo[]   = {SL_L2, KC_QUOT, COMBO_END};

combo_t key_combos[COMBO_COUNT] = {
    COMBO(lparen_combo, KC_LPRN),
    COMBO(rparen_combo, KC_RPRN),
    COMBO(lbkt_combo,   KC_LBRC),
    COMBO(rbkt_combo,   KC_RBRC),
    COMBO(lbrace_combo, KC_LCBR),
    COMBO(rbrace_combo, KC_RCBR),
    COMBO(pipe_combo,   KC_PIPE),
    COMBO(bslh_combo,   KC_BSLS),
};

// ── Require-prior-idle: bypass hold-tap during typing ──
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        last_keycode = keycode;
        uint16_t elapsed = timer_elapsed(last_key_time);

        switch (keycode) {
            case GU_SPC:
                if (elapsed < RPI_SPACE) {
                    tap_code(KC_SPC);
                    return false;
                }
                break;
            case GU_BSP:
                if (elapsed < RPI_BKSP) {
                    tap_code(KC_BSPC);
                    return false;
                }
                break;
            case Z_L1:
                if (elapsed < RPI_Z) {
                    tap_code(KC_Z);
                    return false;
                }
                break;
            case SL_L2:
                if (elapsed < RPI_SLASH) {
                    tap_code(KC_SLSH);
                    return false;
                }
                break;
            case ESC_L2:
                if (elapsed < RPI_ESC) {
                    tap_code(KC_ESC);
                    return false;
                }
                break;
            case MIN_L1:
                if (elapsed < RPI_MINUS) {
                    tap_code(KC_MINS);
                    return false;
                }
                break;
        }

        last_key_time = timer_read();
    }
    return true;
}

// ── Per-key tapping term ──
uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case GU_BSP:  return 100;
        case GU_SPC:  return 150;
        case Z_L1:
        case SL_L2:   return 120;
        case AL_DEL:
        case AL_ENT:  return 130;
        default:       return TAPPING_TERM;
    }
}

// ── Per-key permissive hold (pinky layer-taps only) ──
bool get_permissive_hold(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case ESC_L2:
        case Z_L1:
        case MIN_L1:
        case SL_L2:
            return true;
        default:
            return false;
    }
}

// ── Per-key hold on other key press (yeet only) ──
bool get_hold_on_other_key_press(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case GU_BSP: return true;
        default:      return false;
    }
}

// ── Per-key retro tapping (yeet only) ──
bool get_retro_tapping(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case GU_BSP: return true;
        default:      return false;
    }
}

// ── Per-key quick tap term ──
uint16_t get_quick_tap_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case GU_SPC:  return 90;
        case Z_L1:
        case SL_L2:   return 80;
        default:       return QUICK_TAP_TERM;
    }
}

// ── OLED display ──
#ifdef OLED_ENABLE

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    if (!is_keyboard_master()) {
        return OLED_ROTATION_180;
    }
    return rotation;
}

static void render_layer(void) {
    oled_write_P(PSTR("Layer: "), false);
    switch (get_highest_layer(layer_state)) {
        case 0:
            oled_write_ln_P(PSTR("Base"), false);
            break;
        case 1:
            oled_write_ln_P(PSTR("Symbols"), false);
            break;
        case 2:
            oled_write_ln_P(PSTR("Nav/Fn"), false);
            break;
        default:
            oled_write_ln_P(PSTR("???"), false);
            break;
    }
}

static void render_keycode(void) {
    oled_write_P(PSTR("Key: 0x"), false);
    // Print last keycode as 4-digit hex
    static const char hex[] = "0123456789ABCDEF";
    char buf[5];
    buf[0] = hex[(last_keycode >> 12) & 0xF];
    buf[1] = hex[(last_keycode >> 8)  & 0xF];
    buf[2] = hex[(last_keycode >> 4)  & 0xF];
    buf[3] = hex[ last_keycode        & 0xF];
    buf[4] = '\0';
    oled_write_ln(buf, false);
}

static void render_wpm(void) {
    oled_write_P(PSTR("WPM: "), false);
    uint8_t wpm = get_current_wpm();
    char buf[4];
    buf[0] = (wpm >= 100) ? ('0' + wpm / 100) : ' ';
    buf[1] = (wpm >= 10)  ? ('0' + (wpm / 10) % 10) : ' ';
    buf[2] = '0' + wpm % 10;
    buf[3] = '\0';
    oled_write_ln(buf, false);
}

bool oled_task_user(void) {
    if (is_keyboard_master()) {
        render_layer();
        render_keycode();
        render_wpm();
    } else {
        render_wpm();
        render_layer();
    }
    return false;
}

#endif
