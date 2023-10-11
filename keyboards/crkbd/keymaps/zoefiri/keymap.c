/*
Copyright 2019 @foostan
Copyright 2020 Drashna Jaelre <@drashna>

This program is free oftware: you can redistribute it and/or modify
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

#include "enum.h"
#include "pgm.h"
#include "rgb.h"
#include "sync.h"

#include "print.h"
#include "quantum.h"
#include "split_util.h"
#include "transactions.h"
#include "rgb_matrix.h"

#include QMK_KEYBOARD_H

// {{{
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
      QK_BOOT, CST_DBG, XXXXXXX, XXXXXXX, XXXXXXX, BL_TOGG,                       SV_PGM,  PG_PGM, XXXXXXX, XXXXXXX, XXXXXXX,  XX_PGM,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
   RGB_TOG,TD(ZRGB_CYC),RGB_SAI, RGB_VAI, XXXXXXX, XXXXXXX,                     DEL_PGM, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
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
// }}}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    state.last_last_key = state.last_key;
    state.last_key = abs(KC_A - keycode);
    uint32_t start_time = timer_read32();

    // process custom modes
    if(state.custom_mode_enabled) {
        switch(state.mode){
            case PGM_PG_SELECTING:
            case PGM_PG_TARGETING:
                pgm_program(&state, keycode, record);
                return false;
            case NORMAL:
                break;
        }
    }

    // process normal QMK keypresses
    else {
        switch(keycode) {
            case CST_DBG:

                return true;

            case SV_PGM:
                eeconfig_update_user_datablock(&user_config);
                return true;

            case PG_PGM:
                if(state.mode == NORMAL && record->event.pressed) {
                    state.mode = PGM_PG_SELECTING;
                    state.premode_layer = 7;
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

            MACRO_PGMS()

            default:
                return true;
        }
    }

    int end_time = timer_read32()-start_time;
    uprintf("KEYPROCESSING elapsed: %d", end_time);
    return false;
}

// set layer state
layer_state_t layer_state_set_user(layer_state_t lstate) {
    state.main_active_layer = get_highest_layer(lstate);
    return lstate;
}

// sync and RGB stuff
void housekeeping_task_user(void) {
    sync_boards(&state);

    // TODO: rgb states
    // set_rgb_state(&state);
}

void user_sync_a_sister_handler(uint8_t our_len, const void* our_data, uint8_t sister_len, void* sister_data) {
    if (our_len == sizeof(state_t)) {
        memcpy(&state, our_data, sizeof(state_t));
    }
}

int yx_led_map_main[4][6] = {
    {18, 17, 12, 11, 4, 3},
    {19, 16, 13, 10, 5, 2},
    {20, 15, 14, 9,  6, 1},
    {-1, -1, -1, 8,  7, 0}
};

int yx_led_map_sister[4][6] = {
    {39, 38, 33, 32, 25, 24},
    {40, 37, 34, 31, 26, 23},
    {41, 36, 35, 30, 27, 22},
    {-1, -1, -1, 29, 28, 21}
};

// int idx_bypass[48] =
//     {39, 38, 33, 32, 25, 24, 39, 38, 33, 32, 25, 24
//      40, 37, 34, 31, 26, 23, 40, 37, 34, 31, 26, 23
//      41, 36, 35, 30, 27, 22, 41, 36, 35, 30, 27, 22
//      -1, -1, -1, 29, 28, 21, -1, -1, -1, 29, 28, 21}

void keyboard_post_init_user(void) {
    // Initialize RGB to static black
    // rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
    // rgb_matrix_sethsv_noeeprom(HSV_OFF);

    // Read the user config from EEPROM
    // eeconfig_read_user_datablock(&user_config);

    // srand
    srand(0);

    // setup state stuff
    state.last_frame_enabled = malloc(sizeof(int) * 48);
    state.isSister = !is_keyboard_master();
    state.yx_led_map = state.isSister ? &yx_led_map_main : &yx_led_map_main;

    // register sister board handler
    transaction_register_rpc(USER_SYNC_A, user_sync_a_sister_handler);
    printf("hiiii! \n");
}

