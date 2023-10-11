#include <stdio.h>
#include <stdlib.h>

#include "state.h"

#include "transactions.h"

void sync_boards(state_t *state) {
    if (is_keyboard_master()) {
        bool needs_sync = false;
        if (memcmp(state, &last_state, sizeof(state_t))) {
            needs_sync = true;
            memcpy(&last_state, state, sizeof(state_t));
        }
        if (timer_elapsed32(last_sync) > 500) {
            needs_sync = true;
        }

        // Perform the sync if requested
        if (needs_sync) {
            if (transaction_rpc_send(USER_SYNC_A, sizeof(*state), &state)) {
                last_sync = timer_read32();
            }
        }
    }
}
