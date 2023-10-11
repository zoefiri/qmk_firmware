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

#pragma once

#ifndef CONFIG_H
#define CONFIG_H

/* Select hand configuration */

#define MASTER_LEFT
#define DYNAMIC_KEYMAP_LAYER_COUNT 9
// #define MASTER_RIGHT
// #define EE_HANDS
//
#ifdef RGBLIGHT_ENABLE
    #define RGBLIGHT_EFFECT_BREATHING
    #define RGBLIGHT_EFFECT_RAINBOW_MOOD
    #define RGBLIGHT_EFFECT_RAINBOW_SWIRL
    #define RGBLIGHT_EFFECT_SNAKE
    #define RGBLIGHT_EFFECT_KNIGHT
    #define RGBLIGHT_EFFECT_CHRISTMAS
    #define RGBLIGHT_EFFECT_STATIC_GRADIENT
//  #define RGBLIGHT_EFFECT_RGB_TEST
//  #define RGBLIGHT_EFFECT_ALTERNATING
//  #define RGBLIGHT_EFFECT_TWINKLE
    #define RGBLIGHT_LIMIT_VAL 120
    #define RGBLIGHT_HUE_STEP 10
    #define RGBLIGHT_SAT_STEP 17
    #define RGBLIGHT_VAL_STEP 17
#endif

#define OLED_FONT_H "keyboards/crkbd/lib/glcdfont.c"

#define WEAR_LEVELING_LOGICAL_SIZE 245760
#define WEAR_LEVELING_BACKING_SIZE WEAR_LEVELING_LOGICAL_SIZE*2

#define EECONFIG_USER_DATA_SIZE 4000

// #undef WS2812_DI_PIN
// #define WS2812_DI_PIN 25U
// #define WS2812_PIO_USE_PIO1
// #undef RGB_MATRIX_LED_COUNT
// #define RGB_MATRIX_LED_COUNT 2
// #define RGB_MATRIX_LED_PROCESS_LIMIT 2
// #undef RGB_MATRIX_SPLIT
// #define SPLIT_TRANSPORT_MIRROR
// #define RGB_MATRIX_SPLIT {1, 1}

// #undef WS2812_DI_PIN
// #define WS2812_DI_PIN 25
// #undef RGBLED_NUM
// #define RGBLED_NUM 2
// #undef RGBLED_SPLIT
// #define RGBLED_SPLIT {1, 1}

#define SPLIT_TRANSACTION_IDS_USER USER_SYNC_A, USER_SYNC_B, USER_SYNC_C

#endif
