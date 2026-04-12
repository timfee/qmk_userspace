#pragma once
#include QMK_KEYBOARD_H

// ── Key aliases (layer numbers match Vial: L1=symbols, L2=nav) ──
#define ESC_L2  LT(2, KC_ESC)
#define Z_L1    LT(1, KC_Z)
#define MIN_L1  LT(1, KC_MINS)
#define SL_L2   LT(2, KC_SLSH)
#define CT_GRV  LCTL_T(KC_GRV)
#define AL_DEL  LALT_T(KC_DEL)
#define GU_BSP  LGUI_T(KC_BSPC)
#define GU_SPC  RGUI_T(KC_SPC)
#define AL_ENT  RALT_T(KC_ENT)
#define CT_BSL  RCTL_T(KC_BSLS)

// ── Require-prior-idle thresholds (ms) ──
#define RPI_SPACE  150
#define RPI_Z      125
#define RPI_SLASH  150
#define RPI_ESC    125
#define RPI_MINUS  150
#define RPI_BKSP   150
