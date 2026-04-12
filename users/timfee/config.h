#pragma once

// ── Tap-hold ──
#define TAPPING_TERM 150
#define TAPPING_TERM_PER_KEY
#define QUICK_TAP_TERM 90
#define QUICK_TAP_TERM_PER_KEY
#define PERMISSIVE_HOLD_PER_KEY
#define HOLD_ON_OTHER_KEY_PRESS_PER_KEY
#define RETRO_TAPPING
#define RETRO_TAPPING_PER_KEY

// ── Combos ──
#define COMBO_COUNT 8
#define COMBO_TERM 40
#define COMBO_ONLY_FROM_LAYER 0

// ── OLED (SSD1312 on Keebart Corne Choc Pro) ──
// Note: The vial-qmk-corne-choc-pro driver lacks SSD1312 support
// (no OLED_IC_SSD1312, OLED_FLIP_SEGMENT, or configurable charge pump).
// We set what the driver honors here; the SSD1312-specific fixups
// (segment remap + charge pump voltage) are sent via raw I2C commands
// in oled_post_init() inside timfee.c.
#define OLED_DISPLAY_128X64
#define OLED_DISPLAY_ADDRESS 0x3C
#define OLED_BRIGHTNESS 64
#define OLED_TIMEOUT 0
#define OLED_TIMEOUT_USER 60000
#define OLED_FADE_OUT

// ── Split transport ──
#define SPLIT_TRANSPORT_MIRROR
#define SPLIT_WPM_ENABLE
#define SPLIT_WATCHDOG_TIMEOUT 4000
#define SPLIT_TRANSACTION_IDS_USER USER_SYNC_OLED_STATE, USER_SYNC_LASTKEY, USER_SYNC_PRESSES
