#include <stdio.h>

#include "state.h"

// void pgm_exit(state_t *state) {
//
// }

void pgm_program(state_t *state, uint16_t keycode, keyrecord_t *record) {
        switch(state->mode) {
            // get key to program
            case PGM_PG_SELECTING:
                if(record->event.pressed && !(keycode >= MO(1) && keycode <= MO(7))) {
                    if(keycode >= PGM_00 && keycode <= PGM_41) {
                        state->mode = PGM_PG_TARGETING;
                        state->pgm_pgmkey = keycode - PGM_00;
                    }
                    else if(keycode == PG_PGM) {
                        state->mode = NORMAL;
                    }
                    layer_move(0);
                }
                break;

            // get programming target
            case PGM_PG_TARGETING:
                if(record->event.pressed && !(keycode >= MO(1) && keycode <= MO(7))) {
                    if(!(keycode >= PGM_00 && keycode <= PG_PGM)) {
                        user_config.pgm_layer[state->pgm_pgmkey] = keycode;
                    }
                    state->mode = NORMAL;
                    layer_move(state->premode_layer);
                }
                break;
            case NORMAL:
                break;
        }
        return;
}
