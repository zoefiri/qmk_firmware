/*
Copyright 2019 @foostan
Copyright 2020 Drashna Jaelre <@drashna>

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

#include <stdio.h>
#include <stdlib.h>

#include "print.h"
#include "quantum.h"
#include "split_util.h"
#include "transactions.h"

#include QMK_KEYBOARD_H

enum my_keycodes {
  PGM_00 = SAFE_RANGE,
  PGM_01, PGM_02, PGM_03, PGM_04, PGM_05, PGM_06,
  PGM_07, PGM_08, PGM_09, PGM_10, PGM_11, PGM_12,
  PGM_13, PGM_14, PGM_15, PGM_16, PGM_17, PGM_18,
  PGM_19, PGM_20, PGM_21, PGM_22, PGM_23, PGM_24,
  PGM_25, PGM_26, PGM_27, PGM_28, PGM_29, PGM_30,
  PGM_31, PGM_32, PGM_33, PGM_34, PGM_35, PGM_36,
  PGM_37, PGM_38, PGM_39, PGM_40, PGM_41,
  SV_PGM, XX_PGM,DEL_PGM, PG_PGM, TO_PGM
};

typedef union {
  uint8_t raw[2048];
  struct {
      uint16_t pgm_layer[42];

      uint8_t  padding[1964];
  };
} user_config_t;

typedef struct {
    uint8_t pgm_setting_stage;
    uint8_t pgm_setting;

    uint16_t main_active_layer;
} state_t;

user_config_t user_config;

state_t state;
state_t last_state;
state_t sister_state;
uint32_t last_sync;

uint16_t *pgm_layer;

// We're going to erase and reprogram a region 256k from the start of flash.
// Once done, we can access this at XIP_BASE + 512k.
#define FLASH_TARGET_OFFSET (512 * 1024)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
       KC_TAB,    KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,                         KC_Y,    KC_U,    KC_I,    KC_O,   KC_P,  KC_BSPC,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_LCTL,    KC_A,    KC_S,    KC_D,    KC_F,    KC_G,                         KC_H,    KC_J,    KC_K,    KC_L, KC_SCLN, KC_QUOT,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_LSFT,    KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,                         KC_N,    KC_M, KC_COMM,  KC_DOT, KC_SLSH,  KC_ESC,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                          KC_LGUI,   MO(1),  KC_SPC,     KC_ENT,   MO(2), KC_RALT
                                      //`--------------------------'  `--------------------------'

  ),

    [1] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
       KC_TAB,    KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                         KC_6,    KC_7,    KC_8,    KC_9,    KC_0, KC_BSPC,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_LCTL, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      KC_LEFT, KC_DOWN,   KC_UP,KC_RIGHT, XXXXXXX, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_LSFT, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                          KC_LGUI, _______,  KC_SPC,     KC_ENT,   MO(3), KC_RALT
                                      //`--------------------------'  `--------------------------'
  ),

    [2] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
       KC_TAB, KC_EXLM,   KC_AT, KC_HASH,  KC_DLR, KC_PERC,                      KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_BSPC,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_LCTL, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      KC_MINS,  KC_EQL, KC_LBRC, KC_RBRC, KC_BSLS,  KC_GRV,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_LSFT, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      KC_UNDS, KC_PLUS, KC_LCBR, KC_RCBR, KC_PIPE, KC_TILD,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                          KC_LGUI,   MO(3),  KC_SPC,     KC_ENT, _______, KC_RALT
                                      //`--------------------------'  `--------------------------'
  ),

    [3] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
      QK_BOOT, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, BL_TOGG,                       SV_PGM,  PG_PGM, XXXXXXX, XXXXXXX, XXXXXXX,  XX_PGM,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      RGB_TOG, RGB_HUI, RGB_SAI, RGB_VAI, XXXXXXX, XXXXXXX,                      DEL_PGM, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      RGB_MOD, RGB_HUD, RGB_SAD, RGB_VAD, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,  TO_PGM,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                          KC_LGUI, _______,  KC_SPC,     KC_ENT, _______, KC_RALT
                                      //`--------------------------'  `--------------------------'
  ),

    [4] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
        QK_BOOT, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      RGB_TOG, RGB_HUI, RGB_SAI, RGB_VAI, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      RGB_MOD, RGB_HUD, RGB_SAD, RGB_VAD, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                          KC_LGUI, _______,  KC_SPC,     KC_ENT, _______, KC_RALT
                                      //`--------------------------'  `--------------------------'
  ),

    [5] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
      QK_BOOT, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      RGB_TOG, RGB_HUI, RGB_SAI, RGB_VAI, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      RGB_MOD, RGB_HUD, RGB_SAD, RGB_VAD, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                          KC_LGUI, _______,  KC_SPC,     KC_ENT, _______, KC_RALT
                                      //`--------------------------'  `--------------------------'
  ),

    [6] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
        QK_BOOT, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      RGB_TOG, RGB_HUI, RGB_SAI, RGB_VAI, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      RGB_MOD, RGB_HUD, RGB_SAD, RGB_VAD, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                          KC_LGUI, _______,  KC_SPC,     KC_ENT, _______, KC_RALT
                                      //`--------------------------'  `--------------------------'
  ),

    [7] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
       PGM_00,  PGM_01,  PGM_02,  PGM_03,  PGM_04,  PGM_05,                       PGM_06,  PGM_07,  PGM_08,  PGM_09,  PGM_10,  PGM_11,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
       PGM_12,  PGM_13,  PGM_14,  PGM_15,  PGM_16,  PGM_17,                       PGM_18,  PGM_19,  PGM_20,  PGM_21,  PGM_22,  PGM_23,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
       PGM_24,  PGM_25,  PGM_26,  PGM_27,  PGM_28,  PGM_29,                        TO(0),  PGM_31,  PGM_32,  PGM_33,  PGM_34,  PGM_35,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                           PGM_36,  PGM_37,  PGM_38,     PGM_39,  PGM_40,  PGM_41
                                      //`--------------------------'  `--------------------------'

  )
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    uprintf("keypress: %d, basePG: %d, pgPG: %d, stage: %d\n", keycode, PGM_00, PG_PGM, state.pgm_setting_stage);
    switch(state.pgm_setting_stage) {
    // if waiting for PGM key to bind
        case 1:
            uprintf("state 1\n");
            if(record->event.pressed && !(keycode >= MO(1) && keycode <= MO(7))) {
                if(keycode >= PGM_00 && keycode <= PGM_41) {
                    state.pgm_setting_stage = 2;
                    state.pgm_setting = keycode - PGM_00;
                }
                else if(keycode == PG_PGM) {
                    state.pgm_setting_stage = 0;
                }
                layer_move(0);
            }
            return true;

        // if waiting for key to program for PGM key
        case 2:
            uprintf("state 2\n");
            if(record->event.pressed && !(keycode >= MO(1) && keycode <= MO(7))) {
                if(!(keycode >= PGM_00 && keycode <= PG_PGM)) {
                    user_config.pgm_layer[state.pgm_setting] = keycode;
                }
                state.pgm_setting_stage = 0;
            }
            return true;

        default:
            uprintf("state 0\n");
            // PGM shit
            if(keycode >= PGM_00 && keycode <= PGM_41) {
                uprintf("state 0 pgmhandle\n");
                if(record->event.pressed) {
                    register_code(
                            user_config.pgm_layer[keycode - PGM_00]
                    );
                }
                else {
                    unregister_code(
                            user_config.pgm_layer[keycode - PGM_00]
                    );
                }
                return false;
            }

            // handle normal keys
            switch(keycode) {
                case SV_PGM:
                    uprintf("state 0 pgmdump\n");
                    eeconfig_update_user_datablock(&user_config);
                    return true;

                case PG_PGM:
                    uprintf("state 0 pgpgm\n");
                    if(!state.pgm_setting_stage && record->event.pressed) {
                        state.pgm_setting_stage = 1;
                        layer_move(7);
                    }
                    return true;

                case XX_PGM:
                    for(int i=0; i<42; i++) {
                        user_config.pgm_layer[i] = KC_NO;
                    }
                    eeconfig_update_user_datablock(&user_config);
                    return true;

                case TO_PGM:
                    layer_move(7);
                    return true;

                default:
                    return true;
        }
    }
}

layer_state_t layer_state_set_user(layer_state_t lstate) {
    state.main_active_layer = get_highest_layer(lstate);
    uprintf("set layer state to %d\n", state.main_active_layer);
    return lstate;
}

void housekeeping_task_user(void) {
    if (is_keyboard_master()) {
        bool needs_sync = false;
        if (memcmp(&state, &last_state, sizeof(state_t))) {
            needs_sync = true;
            memcpy(&last_state, &state, sizeof(state_t));
        }
        if (timer_elapsed32(last_sync) > 500) {
            needs_sync = true;
        }

        // Perform the sync if requested
        if (needs_sync) {
            if (transaction_rpc_send(USER_SYNC_A, sizeof(state), &state)) {
                last_sync = timer_read32();
            }
        }
    }

    switch(state.pgm_setting_stage) {
    case 1:
        rgblight_setrgb(117, 157, 10);
        break;

    case 2:
        rgblight_setrgb(127, 127, 0);
        break;

    default:
        uprintf("switching rgb for layer: %d\n", state.main_active_layer);
        switch(state.main_active_layer) {
            case 0:
                rgblight_setrgb(107, 107, 40);
                break;
            case 7:
                rgblight_setrgb(200, 55, 00);
                break;
            default:
                rgblight_setrgb(255, 255, 255);
                break;
        }
        break;
    }
}

void user_sync_a_sister_handler(uint8_t our_len, const void* our_data, uint8_t sister_len, void* sister_data) {
    if (our_len == sizeof(state_t)) {
        memcpy(&state, our_data, sizeof(state_t));
    }
}

void keyboard_post_init_user(void) {
    // Initialize RGB to static black
    rgblight_enable_noeeprom();
    rgblight_sethsv_noeeprom(HSV_BLACK);
    rgblight_mode_noeeprom(RGBLIGHT_MODE_STATIC_LIGHT);

    // Read the user config from EEPROM
    eeconfig_read_user_datablock(&user_config);

    // register sister board handler
    transaction_register_rpc(USER_SYNC_A, user_sync_a_sister_handler);
}


#ifdef RGB_MATRIX_ENABLE

  // Logical Layout
  // Columns
  // Left
  // 0  1  2  3  4  5
  //                   ROWS
  // 25 24 19 18 11 10   0
  //    03    02    01
  // 26 23 20 17 12 09   1
  //    04    05    06
  // 27 22 21 16 13 08   2
  //
  //          15 14 07   3
  //
  // Right
  // 0  1  2  3  4  5
  //                    ROWS
  // 25 24 19 18 11 10   4
  //    03    02    01
  // 26 23 20 17 12 09   5
  //    04    05    06
  // 27 22 21 16 13 08   6
  //
  //          15 14 07   7
  //
  // Physical Layout
  // Columns
  // 0  1  2  3  4  5  6  7  8  9  10 11 12 13
  //                                           ROWS
  // 25 24 19 18 11 10       10 11 18 19 24 25  0
  //    03    02    01       01    02    03
  // 26 23 20 17 12 09       09 12 17 20 23 26  1
  //    04                               04
  // 27 22 21 16 13 08       08 13 16 21 22 27  2
  //          05    06       06    05
  //           15 14 07     07 14 15              3

led_config_t g_led_config = { {
        {  1,  1,  1,  1,  1, 1 },
        {  1,  1,  1,  1,  1, 1 },
        {  1,  1,  1,  1,  1, 1 },
        { NO_LED, NO_LED, NO_LED,  1,  1,  1 },
        {  0,  0,  0,  0,  0, 0 },
        {  0,  0,  0,  0,  0, 0 },
        {  0,  0,  0,  0,  0, 0 },
        { NO_LED, NO_LED, NO_LED,  0,  0,  0 }
    },
    {
        { 0, 0 },
        { 6, 0 }
    },
    {
        0x0f, 0x0f
    }
};

bool rgb_matrix_indicators_user(void) {
    switch(state.pgm_setting_stage) {
    case 1:
        rgb_matrix_set_color_all(117, 157, 10);
        break;

    case 2:
        rgb_matrix_set_color_all(127, 127, 0);
        break;

    default:
        uprintf("switching rgb for layer: %d\n", state.main_active_layer);
        switch(state.main_active_layer) {
            case 0:
                rgb_matrix_set_color_all(107, 107, 40);
                break;
            case 7:
                rgb_matrix_set_color_all(200, 55, 00);
                break;
            default:
                rgb_matrix_set_color_all(255, 255, 255);
                break;
        }
        break;
    }
    return false;
}


#endif

