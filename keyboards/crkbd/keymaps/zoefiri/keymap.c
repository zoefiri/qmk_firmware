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
#include "quantum_keycodes.h"
#include "rgb.h"
#include "sync.h"
#include "moonscrape.h"

#include "print.h"
#include "signal.h"
#include "timer.h"
#include "quantum.h"
#include "split_util.h"
#include "transactions.h"
#include "rgb_matrix.h"

#include QMK_KEYBOARD_H

uint32_t time;

uint32_t animframe;

// {{{
#define HDG(KC) MT(MOD_LGUI, KC)
#define HDA(KC) MT(MOD_LALT, KC)
#define HDC(KC) MT(MOD_LCTL, KC)
#define HDS(KC) MT(MOD_LSFT, KC)
#define HDRG(KC) MT(MOD_RGUI, KC)
#define HDRA(KC) MT(MOD_RALT, KC)
#define HDRC(KC) MT(MOD_RCTL, KC)
#define HDRS(KC) MT(MOD_RSFT, KC)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
     KC_TAB,    KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,                         KC_Y,    KC_U,    KC_I,    KC_O,   KC_P,  KC_BSPC,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
     KC_LCTL,HDG(KC_A),HDA(KC_S),HDC(KC_D),HDS(KC_F),KC_G,                        KC_H,  HDS(KC_J),HDC(KC_K),HDA(KC_L),HDG(KC_SCLN),KC_QUOT,
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
      KC_LCTL,HDG(KC_NO),HDA(KC_NO),HDC(KC_NO),HDS(KC_NO), XXXXXXX,              KC_LEFT, KC_DOWN,   KC_UP,KC_RIGHT, XXXXXXX, XXXXXXX,
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
      KC_LCTL,HDG(KC_NO),HDA(KC_NO),HDC(KC_NO),HDS(KC_NO), XXXXXXX,              KC_MINS,HDS(KC_EQL),HDC(KC_LBRC),HDA(KC_RBRC),HDG(KC_BSLS),KC_GRV,
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
        // If below SAFE_RANGE, returning now is fine.
        if (keycode < SAFE_RANGE) {
            return true;
        }

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

void user_sync_a_sister_handler(uint8_t main_len, const void* main_data, uint8_t sister_len, void* sister_data) {
    if (main_len == sizeof(state_t)) {
        memcpy(&state, main_data, main_len);
    }
}

// int yx_led_map_main[4][6] = {
//     {18, 17, 12, 11, 4, 3},
//     {19, 16, 13, 10, 5, 2},
//     {20, 15, 14, 9,  6, 1},
//     {-1, -1, -1, 8,  7, 0}
// };

// int yx_led_map_sister[4][6] = {
//     {39, 38, 33, 32, 25, 24},
//     {40, 37, 34, 31, 26, 23},
//     {41, 36, 35, 30, 27, 22},
//     {-1, -1, -1, 29, 28, 21}
// };

// int idx_bypass[48] =
//     {39, 38, 33, 32, 25, 24, 39, 38, 33, 32, 25, 24
//      40, 37, 34, 31, 26, 23, 40, 37, 34, 31, 26, 23
//      41, 36, 35, 30, 27, 22, 41, 36, 35, 30, 27, 22
//      -1, -1, -1, 29, 28, 21, -1, -1, -1, 29, 28, 21}

void segfault_handler(int dummy) {
    register_code16(QK_BOOT);
    unregister_code16(QK_BOOT);
}

void keyboard_post_init_user(void) {
    // init segfault handler
    signal(SIGSEGV, segfault_handler);
    signal(SIGTERM, segfault_handler);
    signal(SIGINT, segfault_handler);
    signal(SIGKILL, segfault_handler);
    signal(SIGQUIT, segfault_handler);

    // Initialize RGB to static black
    rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
    rgb_matrix_sethsv_noeeprom(HSV_OFF);

    // Read the user config from EEPROM
    // eeconfig_read_user_datablock(&user_config);
    debug_enable = true;

    // srand
    srand(0);

    // setup state stuff
    state.last_frame_enabled = malloc(sizeof(int) * 48);
    state.isSister = !is_keyboard_master();
    state.oled_msg_main = malloc(sizeof(char)*100);
    state.oled_msg_sister = malloc(sizeof(char)*100);
    state.randmsg = malloc(sizeof(char)*10);
    sprintf(state.randmsg, "r:%d\n", rand()%100);

    // register sister board handler
    transaction_register_rpc(USER_SYNC_A, user_sync_a_sister_handler);
    printf("hiiii! \n");
}

bool oled_task_user(void) {
    if (animframe == 0) {
        oled_write_raw_P(frame, ANIM_SIZE);
    }

    animframe += 1;
    change_frame_bytewise(animframe % IDLE_FRAMES);

    return false;
}

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    return OLED_ROTATION_270;  // flips the display 180 degrees if offhand
}

led_config_t g_led_config = {
    {
        {18, 17, 12, 11, 4, 3},
        {19, 16, 13, 10, 5, 2},
        {20, 15, 14, 9, 6, 1},
        {NO_LED, NO_LED, NO_LED, 8, 7, 0},

        {39, 38, 33, 32, 25, 24},
        {40, 37, 34, 31, 26, 23},
        {41, 36, 35, 30, 27, 22},
        {NO_LED, NO_LED, NO_LED, 29, 28, 21}},
    {{95, 63}, {85, 39}, {85, 21}, {85, 4}, {68, 2}, {68, 19}, {68, 37}, {80, 58}, {60, 55}, {50, 35}, {50, 13}, {50, 0}, {33, 3}, {33, 20}, {33, 37}, {16, 42}, {16, 24}, {16, 7}, {0, 7}, {0, 24}, {0, 41}, {129, 63}, {139, 39}, {139, 21}, {139, 4}, {156, 2}, {156, 19}, {156, 37}, {144, 58}, {164, 55}, {174, 35}, {174, 13}, {174, 0}, {191, 3}, {191, 20}, {191, 37}, {208, 42}, {208, 24}, {208, 7}, {224, 7}, {224, 24}, {224, 41}},
    {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4}};


