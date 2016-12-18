#ifndef turn_command_h_
#define turn_command_h_

#include "boe_bot_command_base.h"


/**
 * Command for 90deg rotation of the robot in preferred directions.
 */
class turn_command : public boe_bot_command_base {

    /**
     * Defines direction of the rotation.
     */
    bool left;

    /**
     * Defines if the robot can still see the line in original direction.
     */
    bool leavingFirstLine;

public:

    /**
     * Creates a new rotation command for given robot in selected direction.
     *
     * @param left_p Direction of the rotation.
     * @param robot_p Robot to be commanded.
     * @param final_location_p Desired final position.
     */
    turn_command(bool left_p, boe_bot *robot_p, location final_location_p);

    /**
     * Continue to do this command.
     *
     * @param time_elapsed Absolute elapsed time from the start of the dance.
     */
    virtual void update(const time_t &time_elapsed) override;

    /**
     * Force this command to stop regardless of its state.
     *
     * @return //TODO ?
     */
    virtual bool force_stop() override;
};



//class turn

inline turn_command::turn_command(bool left_p, boe_bot *robot_p, location final_location_p) : boe_bot_command_base(
        robot_p, final_location_p), left(left_p), leavingFirstLine(true) {};

//NOTE: if planing is correct we don't have to take care of corners
void turn_command::update(const time_t &time_elapsed) {
    if (is_done()) {
        return;
    }

    /* First call to this function */
    if (state == command_state::PREPARED) {
        state = command_state::IN_PROCESS;
    }

    if (leavingFirstLine) {
        if (left)
            robot->in_place_left();
        else
            robot->in_place_right();

        if (!robot->get_sensors().middle())
            leavingFirstLine = false;
    } else {
        /* Is robot aligned to next line? */
        if (!robot->get_sensors().second_left() && robot->get_sensors().middle() && !robot->get_sensors().second_right()) {
            robot->stop();

            state = command_state::WAITING;
        }
        //NOTE: robot can move a little bit even if he stopped -> this condition outside of previous one
        if (state == command_state::WAITING && (!time_constrain || time_constrain >= time_elapsed)) {
            finish();
        }
    }

};

inline bool turn_command::force_stop() {
    finish();
    //TODO co vrací?
    return true;
};

#endif
