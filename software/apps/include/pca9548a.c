
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "pca9548a.h"
#include "apds-9960.h"
#include "state.h"
#include "i2c_utils.h"

void mux_set_active(uint8_t ctrl_byte) {
    i2c_write_byte(PCA_ADDR, &ctrl_byte, 0);
}

void mux_activate_left() {
    mux_set_active(MUX_LEFT);
}

void mux_activate_right() {
    mux_set_active(MUX_RIGHT);
}

void mux_init_sensors() {
  mux_activate_left();
  apds_init(true);
  mux_activate_right();
  apds_init(false);
}

void mux_update_line_state() {

    // if (mux_check_end()) {
    //     return;
    // }

    mux_activate_left();
    bool left_line_triggered = check_over_line(true);
    mux_activate_right();
    bool right_line_triggered = check_over_line(false);

    // Check if you're hitting the line on
    // the left, the right, neither, or at a junction
    if (left_line_triggered && !right_line_triggered) {
        switch (state_line_trigger) {
            case STATE_LEFT_TRIGGERED:
                // Do nothing, already up to date
                break;
            default:
                printf("Sensors say turn left!\n");
                state_line_trigger = STATE_LEFT_TRIGGERED;
                state_line_changed = true;
                break;
        }
    } else if (!left_line_triggered && right_line_triggered) {
        switch (state_line_trigger) {
            case STATE_RIGHT_TRIGGERED:
                // Do nothing, already up to date
                break;
            default:
                printf("Sensors say turn right!\n");
                state_line_trigger = STATE_RIGHT_TRIGGERED;
                state_line_changed = true;
                break;
        }
    } else if (left_line_triggered && right_line_triggered) {
        switch (state_line_trigger) {
            case STATE_AT_JUNCTION:
                // Do nothing, already up to date
                break;
            default:
                printf("Sensors say at junction!\n");
                state_line_trigger = STATE_AT_JUNCTION;
                state_line_changed = true;
                break;
        }
    } else {
        switch (state_line_trigger) {
            case STATE_NO_TRIGGERS:
                // Do nothing, already up to date
                break;
            default:
                printf("Sensors say drive straight!\n");
                state_line_trigger = STATE_NO_TRIGGERS;
                state_line_changed = true;
                break;
        }
    }

}

bool mux_check_end() {

    mux_activate_left();
    bool left_line_end = detect_color(GREEN, state_red_threshold);
    mux_activate_right();
    bool right_line_end = detect_color(GREEN, state_red_threshold);

    return left_line_end || right_line_end;

}
