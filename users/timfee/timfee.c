#include "timfee.h"

#ifdef OLED_ENABLE
#include "bitmaps.h"
#include "transactions.h"
#endif

// ── State for require-prior-idle ──
static uint16_t last_key_time = 0;

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

// ═══════════════════════════════════════════════════════════════════
// OLED state (compiled only when OLED_ENABLE is set)
// ═══════════════════════════════════════════════════════════════════
#ifdef OLED_ENABLE

static const uint8_t OLED_WIDTH = OLED_DISPLAY_HEIGHT;

static const char PROGMEM QMK_LOGO_1[] = { 0x81, 0x82, 0x83, 0x84, 0x00 };
static const char PROGMEM QMK_LOGO_2[] = { 0xA1, 0xA2, 0xA3, 0xA4, 0x00 };
static const char PROGMEM QMK_LOGO_3[] = { 0xC1, 0xC2, 0xC3, 0xC4, 0x00 };

typedef struct { bool oled_on; }    oled_state_m2s_t;
typedef struct { uint16_t keycode; } lastkey_m2s_t;
typedef struct { uint32_t left; uint32_t right; } presses_m2s_t;

static bool     g_oled_init_done = false;
static uint8_t  g_oled_max_char;
static uint32_t g_user_ontime    = 0;
static uint16_t g_last_keycode   = KC_NO;
static uint32_t g_press_left     = 0;
static uint32_t g_press_right    = 0;
static oled_state_m2s_t g_remote_oled_state = { false };
static presses_m2s_t    g_remote_presses    = { 0, 0 };

// ── Charge-pump enable pin (hardware-specific) ──
static inline pin_t get_charge_pump_enable_pin(void) {
    return is_keyboard_left() ? GP5 : GP25;
}

// ── Raw-byte blit for 16×16 icons ──
void oled_blit_16x16_P(const char *icon, uint8_t x, uint8_t page) {
    for (uint8_t i = 0; i < 16; i++) {
        oled_write_raw_byte(pgm_read_byte(&icon[i]),      page       * OLED_WIDTH + x + i);
        oled_write_raw_byte(pgm_read_byte(&icon[16 + i]), (page + 1) * OLED_WIDTH + x + i);
    }
}

// ── Helpers ──
static uint16_t unwrap_keycode(uint16_t kc) {
    if (kc >= QK_MOD_TAP && kc <= QK_MOD_TAP_MAX)
        return QK_MOD_TAP_GET_TAP_KEYCODE(kc);
    if (kc >= QK_LAYER_TAP && kc <= QK_LAYER_TAP_MAX)
        return QK_LAYER_TAP_GET_TAP_KEYCODE(kc);
    return kc;
}

static uint8_t round_percentage(float x) {
    float f = x + 0.5f;
    uint8_t r = (uint8_t)f;
    if ((f - (float)r) == 0.0f && (r & 1)) r--;
    return r;
}

static void oled_print_right_aligned(const char *text, uint8_t width) {
    uint8_t len = strlen(text);
    uint8_t pad = (len < width) ? (width - len) : 0;
    for (uint8_t i = 0; i < pad; i++) oled_write_P(PSTR(" "), false);
    oled_write(text, false);
}

// ── Info renderers ──
static void print_current_layer(uint8_t row) {
    char buf[8];
    switch (get_highest_layer(layer_state)) {
        case 0:  strcpy(buf, "Base");    break;
        case 1:  strcpy(buf, "Symbols"); break;
        case 2:  strcpy(buf, "Nav");     break;
        default: snprintf(buf, sizeof(buf), "%d", (int)get_highest_layer(layer_state)); break;
    }
    oled_set_cursor(0, row);
    oled_print_right_aligned(buf, g_oled_max_char);
}

static void print_uptime(uint8_t row) {
    uint32_t total_min = timer_read32() / 60000u;
    uint32_t hours   = total_min / 60u;
    uint32_t minutes = total_min % 60u;
    if (hours > 999u) { hours = 999u; minutes = 59u; }
    char buf[8];
    snprintf(buf, sizeof(buf), "%3luh%02lum", hours, minutes);
    oled_set_cursor(0, row);
    oled_print_right_aligned(buf, g_oled_max_char);
}

static void print_wpm(uint8_t row) {
    uint16_t dwpm = (uint16_t)get_current_wpm() * 10u;
    char buf[11];
    snprintf(buf, sizeof(buf), "%3u.%1u WPM", dwpm / 10u, dwpm % 10u);
    oled_set_cursor(0, row);
    oled_print_right_aligned(buf, g_oled_max_char);
}

static void print_balance(uint8_t row, uint8_t pct) {
    char buf[6];
    snprintf(buf, sizeof(buf), "%3u %%", pct);
    oled_set_cursor(0, row);
    oled_print_right_aligned(buf, g_oled_max_char);
}

// ── Split RPC callbacks (slave side) ──
static void user_sync_oled_state_slave(uint8_t in_len, const void *in_data,
                                       uint8_t out_len, void *out_data) {
    if (in_len >= sizeof(oled_state_m2s_t))
        memcpy(&g_remote_oled_state, in_data, sizeof(oled_state_m2s_t));
}

static void user_sync_lastkey_slave(uint8_t in_len, const void *in_data,
                                    uint8_t out_len, void *out_data) {
    if (in_len >= sizeof(lastkey_m2s_t))
        g_last_keycode = ((const lastkey_m2s_t *)in_data)->keycode;
}

static void user_sync_presses_slave(uint8_t in_len, const void *in_data,
                                    uint8_t out_len, void *out_data) {
    if (in_len >= sizeof(presses_m2s_t))
        memcpy(&g_remote_presses, in_data, sizeof(presses_m2s_t));
}

#endif // OLED_ENABLE

// ═══════════════════════════════════════════════════════════════════
// QMK hooks
// ═══════════════════════════════════════════════════════════════════

#ifdef OLED_ENABLE
void keyboard_post_init_user(void) {
    pin_t pen = get_charge_pump_enable_pin();
    gpio_set_pin_output(pen);
    gpio_write_pin_low(pen);
    wait_ms(5);

    transaction_register_rpc(USER_SYNC_OLED_STATE, user_sync_oled_state_slave);
    transaction_register_rpc(USER_SYNC_LASTKEY,    user_sync_lastkey_slave);
    transaction_register_rpc(USER_SYNC_PRESSES,    user_sync_presses_slave);

    if (!is_keyboard_master()) wait_ms(90);
}

void housekeeping_task_user(void) {
    if (is_keyboard_master()) {
        static uint32_t last_sync = 0;
        if (timer_elapsed32(last_sync) > 500) {
            oled_state_m2s_t pkt = { is_oled_on() };
            (void)transaction_rpc_send(USER_SYNC_OLED_STATE, sizeof(pkt), &pkt);
            last_sync = timer_read32();
        }
    }
}
#endif // OLED_ENABLE

// ── Require-prior-idle + OLED keypress tracking ──
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
#ifdef OLED_ENABLE
    g_user_ontime = timer_read32();
#endif

    if (record->event.pressed) {
#ifdef OLED_ENABLE
        // Track every keypress for OLED (before RPI may intercept)
        g_last_keycode = keycode;
        if (record->event.key.row < MATRIX_ROWS / 2) {
            g_press_left++;
        } else {
            g_press_right++;
        }
        if (is_keyboard_master()) {
            lastkey_m2s_t kpkt = { g_last_keycode };
            (void)transaction_rpc_send(USER_SYNC_LASTKEY, sizeof(kpkt), &kpkt);
            presses_m2s_t ppkt = { g_press_left, g_press_right };
            (void)transaction_rpc_send(USER_SYNC_PRESSES, sizeof(ppkt), &ppkt);
        }
#endif

        // Require-prior-idle handling
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

// ═══════════════════════════════════════════════════════════════════
// OLED rendering
// ═══════════════════════════════════════════════════════════════════
#ifdef OLED_ENABLE

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    return OLED_ROTATION_90;
}

static bool oled_post_init(void) {
    if (!g_oled_init_done) {
        g_oled_max_char = oled_max_chars();

        pin_t pen = get_charge_pump_enable_pin();
        gpio_write_pin_high(pen);
        wait_ms(20);

        // The vial-qmk-corne-choc-pro OLED driver does not support
        // OLED_IC_SSD1312, OLED_FLIP_SEGMENT, or configurable charge pump.
        // It hardcodes SEGMENT_REMAP_INV (0xA1) and CHARGE_PUMP 0x14,
        // but the SSD1312 needs SEGMENT_REMAP (0xA0) and charge pump 0x72.
        // Send the correct values as raw I2C commands after driver init.
        static const uint8_t ssd1312_fixup[] = {
            0x00,       // I2C command mode
            0xA0,       // SEGMENT_REMAP: flip segment direction for SSD1312
            0x8D, 0x72, // CHARGE_PUMP: 9.0 V for SSD1312
        };
        oled_send_cmd(ssd1312_fixup, sizeof(ssd1312_fixup));

        oled_clear();

        g_user_ontime = timer_read32();
        g_oled_init_done = true;
    }
    return false;
}

bool oled_task_user(void) {
    oled_post_init();

    // ── Resolve press counts (master has live data, slave gets synced) ──
    uint32_t lpresses, rpresses;
    if (is_keyboard_master()) {
        lpresses = g_press_left;
        rpresses = g_press_right;
    } else {
        lpresses = g_remote_presses.left;
        rpresses = g_remote_presses.right;
    }

    // ── OLED on/off management ──
    if (is_keyboard_master()) {
        uint32_t idle = timer_elapsed32(g_user_ontime);
        if (is_oled_on() && idle > OLED_TIMEOUT_USER) {
            oled_off();
            oled_state_m2s_t pkt = { false };
            (void)transaction_rpc_send(USER_SYNC_OLED_STATE, sizeof(pkt), &pkt);
            return false;
        }
        if (!is_oled_on() && idle <= OLED_TIMEOUT_USER) {
            oled_on();
            oled_state_m2s_t pkt = { true };
            (void)transaction_rpc_send(USER_SYNC_OLED_STATE, sizeof(pkt), &pkt);
        }
    } else {
        if (g_remote_oled_state.oled_on) {
            if (!is_oled_on()) oled_on();
        } else {
            if (is_oled_on()) oled_off();
            return false;
        }
    }

    // ── Compute balance ──
    uint32_t total = lpresses + rpresses;
    if (total == 0) total = 1;
    uint8_t pct_left  = round_percentage((100.0f * lpresses) / total);
    uint8_t pct_right = round_percentage((100.0f * rpresses) / total);

    // ── Left half ──
    if (is_keyboard_left()) {
        oled_set_cursor(0, 0);
        oled_write_P(PSTR("Layer:"), false);
        print_current_layer(1);

        // Lock indicators (16×16 bitmaps)
        led_t led_state = host_keyboard_led_state();
        oled_blit_16x16_P(led_state.num_lock    ? NUM_LOCK_BITMAP    : EMPTY_BITMAP, 0,  3);
        oled_blit_16x16_P(led_state.caps_lock   ? CAPS_LOCK_BITMAP   : EMPTY_BITMAP, 24, 3);
        oled_blit_16x16_P(led_state.scroll_lock ? SCROLL_LOCK_BITMAP : EMPTY_BITMAP, 48, 3);

        oled_set_cursor(0, 7);
        oled_write_P(PSTR("Left:"), false);
        print_balance(8, pct_left);

        oled_set_cursor(0, 10);
        oled_write_P(PSTR("Last Key:"), false);
        oled_set_cursor(0, 11);
        oled_print_right_aligned(get_keycode_string(unwrap_keycode(g_last_keycode)),
                                 g_oled_max_char);

        // QMK logo
        oled_set_cursor(0, 13);  oled_write_P(QMK_LOGO_1, false);
        oled_set_cursor(0, 14);  oled_write_P(QMK_LOGO_2, false);
        oled_set_cursor(0, 15);  oled_write_P(QMK_LOGO_3, false);
        oled_set_cursor(7, 15);  oled_write_P(PSTR("QMK"), false);
    }
    // ── Right half ──
    else {
        oled_set_cursor(0, 0);
        oled_write_P(PSTR("Uptime:"), false);
        print_uptime(1);

        oled_set_cursor(0, 3);
        oled_write_P(PSTR("Avg Speed"), false);
        oled_set_cursor(0, 4);
        oled_write_P(PSTR("(25 s):"), false);
        print_wpm(5);

        oled_set_cursor(0, 7);
        oled_write_P(PSTR("Right:"), false);
        print_balance(8, pct_right);

        // QMK logo
        oled_set_cursor(0, 13);  oled_write_P(QMK_LOGO_1, false);
        oled_set_cursor(0, 14);  oled_write_P(QMK_LOGO_2, false);
        oled_set_cursor(0, 15);  oled_write_P(QMK_LOGO_3, false);
        oled_set_cursor(7, 15);  oled_write_P(PSTR("QMK"), false);
    }

    return false;
}

#endif // OLED_ENABLE
