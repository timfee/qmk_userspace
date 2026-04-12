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

// ── OLED (matches Keebart vial_oled reference) ──
#define OLED_IC OLED_IC_SSD1312
#define OLED_DISPLAY_128X64
#define OLED_FLIP_SEGMENT
#define OLED_DISPLAY_ADDRESS 0x3C
#define OLED_CHARGE_PUMP_VALUE 0x72
#define OLED_BRIGHTNESS 64
#define OLED_TIMEOUT 0
#define OLED_FADE_OUT

// ── Split sync ──
#define SPLIT_OLED_ENABLE
#define SPLIT_WPM_ENABLE
#define SPLIT_LAYER_STATE_ENABLE
