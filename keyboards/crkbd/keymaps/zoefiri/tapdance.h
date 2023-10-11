#pragma once

#ifndef TAPDANCE_H
#define TAPDANCE_H

#include "quantum.h"

// tapdance states
typedef enum {
  SINGLE_TAP,
  SINGLE_HOLD,
} td_state_t;

// function to determine the current tapdance state
int cur_dance (tap_dance_state_t *state);

// `finished` and `reset` functions for each tapdance keycode
//
// zrgb_cyc
void zrgb_cyc_finished (tap_dance_state_t *state, void *user_data);
void zrgb_cyc_reset (tap_dance_state_t *state, void *user_data);

#endif
