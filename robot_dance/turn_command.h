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

    /**
     * States whether the middle sensor was not active when the turn started.
     */
    bool middle_missed;

    /**
     * Contains time of start of this turn.
     */
    time_type turn_started;

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
     * Alternative 'constructor' to avoid dynamic allocation.
     *
     * @param left_p Direction of the rotation.
     * @param robot_p Robot to be commanded.
     * @param final_location_p Desired final position.
     */
    void set(bool left, boe_bot *robot_p, const location &final_location_p);

    /**
     * Continue to do this command.
     */
    virtual void update() override;

    virtual char *get_name() override;
};



//class turn_command

inline turn_command::turn_command(bool left_p, boe_bot *robot_p, location final_location_p) : boe_bot_command_base(
        robot_p, final_location_p), left(left_p), leavingFirstLine(true), middle_missed(false), turn_started(0) {};

void turn_command::set(bool left, boe_bot *robot_p, const location &final_location_p) {
    init(robot_p, final_location_p);
    turn_command::left = left;
    leavingFirstLine = true;
    middle_missed = false;
    turn_started = 0;
};

//if planing is correct we don't have to take care of corners,
//because planner should not add two (or three at corners) turn commands
//when robot is on the border, but only single one
void turn_command::update() {
    /* This move is finished */
    if (is_done()) {
        robot->stop();
        return;
    }

    /* First call to this function */
    if (state == command_state::PREPARED) {
        turn_started = millis();
        state = command_state::IN_PROCESS;

        if (left && !robot->get_last_move_encountered_left() || (!left && !robot->get_last_move_encountered_right())) {
            //next turn command will do the job on borders
            Serial.println(F("Turn command skipped due to lack of path"));
            robot->clear_last_move_encounters();
            robot->set_last_move_encountered_left(true);
            robot->set_last_move_encountered_right(true);
            finish();
            return;
        }

        if (!robot->get_sensors().middle()) {
            middle_missed = true;
            Serial.println(F("Middle missed!"));
        }
    }

    /* Allows the middle sensor miss but correction must be done quickly */
    if (middle_missed && millis() - turn_started > 300) {
        middle_missed = false;
    }

    if (state == command_state::IN_PROCESS) {
        if (left) {
            /* Slow down if the target line is approaching */
            if (robot->get_sensors().second_left()) {
                robot->in_place_left_half();
            } else {
                robot->in_place_left();
            }
        } else {
            /* Slow down if the target line is approaching */
            if (robot->get_sensors().second_right()) {
                robot->in_place_right_half();
            } else {
                robot->in_place_right();
            }
        }

        /* It's not possible to turn 'too' quickly */
        if (!robot->get_sensors().middle() && !middle_missed && millis() - turn_started > 400) {
            leavingFirstLine = false;
        }
    }

    if (!leavingFirstLine) {
        /* Is robot aligned to next line? */
        if (/*!robot->get_sensors().first_left() && */!robot->get_sensors().second_left()
            && robot->get_sensors().middle()
            && !robot->get_sensors().first_right()/* && !robot->get_sensors().second_right()*/) {
            robot->stop();

            finish();
        }
    }

};

inline char *turn_command::get_name() {
    if (left) {
        return (char *) "turn command [left]";
    } else {
        return (char *) "turn command [right]";
    }
}

#endif
