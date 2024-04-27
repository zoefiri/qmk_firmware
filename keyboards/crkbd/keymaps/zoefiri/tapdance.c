#include <stdio.h>

#include "quantum.h"
#include "rgb_matrix/rgb_matrix.h"

#include "enum.h"

// tapdance states
typedef enum {
  SINGLE_TAP = 1,
  SINGLE_HOLD,
} td_state_t;

// static td_state_t td_state;

// int cur_dance (tap_dance_state_t *state) {
//     if (state->count == 1) {
//         if (state->interrupted || !state->pressed) {
//             return SINGLE_TAP;
//         }
//         else {
//             return SINGLE_HOLD;
//         }
//     }
//     else {
//         return state->count;
//     }
// }

// `finished` and `reset` functions for each tapdance keycode
//
// zrgb_cyc
// void zrgb_cyc_finished (tap_dance_state_t *state, void *user_data){
//     printf("tapdance! \n");
//     td_state = cur_dance(state);
//     int h = rgb_matrix_get_hue();
//     int s = rgb_matrix_get_sat();
//     int v = rgb_matrix_get_val();
//
//     switch (td_state) {
//         case SINGLE_HOLD:
//             printf("h/s/v %d/%d/%d large adjust\n", h, s ,v);
//             rgb_matrix_sethsv((h+10)%256, s, v);
//             return;
//         default:
//             printf("h/s/v %d/%d/%d minor adjust\n", h, s ,v);
//             rgb_matrix_sethsv((h+1)%256, s, v);
//             return;
//     }
// }
//
// void zrgb_cyc_reset (tap_dance_state_t *state, void *user_data){
//   switch (td_state) {
//     case SINGLE_TAP:
//       break;
//     case SINGLE_HOLD:
//       break;
//   }
//   return;
// }
//
tap_dance_action_t tap_dance_actions[] = {
  // [ZRGB_CYC] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, zrgb_cyc_finished, zrgb_cyc_reset)
};
//
