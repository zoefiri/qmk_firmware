#pragma once

#ifndef STATE_H
#define STATE_H

#include <stdio.h>
#include "enum.h"
#include "rgb.h"

typedef struct {
    int x;
    int y;
} xy;

typedef struct {
    xy* xy;
    int len;
} xy_list;

// TODO: this will be superceded by state once
// user config
typedef union {
    uint8_t raw[2048];
    struct {
        uint16_t pgm_layer[42];

        uint8_t padding[1964];
    };
} user_config_t;

typedef struct {
    /* /////////////////
    //// mode stuff ////
    ///////////////// */
    // custom_mode_enabled is set to true when there is currently some kind of custom mode
    // such as pgm setting enabled
    bool custom_mode_enabled;
    // mode indicates what mode is currently active
    enum modes mode;
    // premode_layer is the layer that was selected before a mode was entered
    uint8_t premode_layer;
    // last key pressed
    uint16_t last_key;
    uint16_t last_last_key;
    // bool to tell if this is the sister board
    bool isSister;

    /* /////////////////
    //// pgm mode   ////
    ///////////////// */
    // pgm_pgmkey is the key currently being programmed
    uint8_t pgm_pgmkey;

    uint16_t main_active_layer;

    /* //////////////////
    //// rgb state   ////
    ////////////////// */
    //
    uint8_t  rgbTicker;
    uint8_t  y_state;
    int*     last_frame_enabled;
    //
    color_t  phys_pixels[PHYS_PIXELS_COUNT];
    //
    int      (*yx_led_map)[4][6];
} state_t;

user_config_t user_config;

// state, and sister board state
state_t state;
state_t sister_state;
// state sync stuff
state_t  last_state;
uint32_t last_sync;

// econfig data locations


/* /////////////////
//// pgm mode   ////
///////////////// */
//
// util
#define EESTATE_START EECONFIG_USER_DATABLOCK // start of eestate block
#define EESTATE_UTIL_SIZE 512 // provisioning 512 bytes for this, for futureproofing purposes mostly
//
// dynamic keymaps
#define EESTATE_KEYMAP_START EESTATE_START + EESTATE_UTIL_SIZE // start of dynam kmap data
#define EESTATE_KEYMAP_COUNT 40 // number of dynam kmap
#define EESTATE_KEYCOUNT 42 // count of keys in dynam kmap
#define EESTATE_META_SIZE 128 // 128 byte provisioned for metadata
#define EESTATE_KEYMAP_SIZE sizeof(uint16_t) * EESTATE_KEYCOUNT; // size of one dynam kmap
#define EESTATE_KEYMAP_FULLSIZE EESTATE_KEYMAP_SIZE + EESTATE_META_SIZE; // size of one dynam kmap with its metadata
#define EESTATE_KEYMAPS_SIZE EESTATE_KEYMAP_FULLSIZE * EESTATE_KEYMAP_COUNT  // size of dynam kmap region

#endif
