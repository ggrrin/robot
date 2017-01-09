#ifndef move_command_h_
#define move_command_h_

#include "boe_bot_command_base.h"


/**
 * Command for transition of the robot along the line in front.
 */
class move_command : public boe_bot_command_base {

    /**
     * Defines if the destination cross was encountered.
     */
    bool cross_encountered;

    /**
     * Defines if the robot is correctly rotated on destination cross.
     */
    bool cross_corrected;

    /**
     * Time in which the destination cross was reached measured in absolute time.
     */
    time_type cross_encountered_time;

    /**
     * Time of start of this command.
     */
    time_type move_started = 0;

    /**
     * Move the robot so that it follows the line.
     */
    void go_straight();

    /**
     * Moves straight until it finds a cross (or partial cross = corner).
     */
    void encounter_cross();

    /**
     * Corrects the robot based on sensor encounter sequence immediately
     * after cross encounter.
     */
    void do_sensors_correction_on_cross();

    /**
     * Move the robot forwards until the sensors are out of the cross.
     * Makes the robot centered on the cross.
     */
    void do_wheels_corrections_on_cross();

public:

    /**
     * Creates a new command to move the robot one tile away from current position.
     *
     * @param robot_p Robot to be commanded.
     * @param final_location_p Desired final position of the robot.
     */
    explicit move_command(boe_bot *robot_p, location final_location_p);

    /**
     * Alternative 'constructor' to avoid dynamic allocation.
     *
     * @param robot_p Robot to be commanded.
     * @param final_location_p Desired final position of the robot.
     */
    void set(boe_bot *robot_p, const location &final_location_p);

    /**
     * Continue to do this command.
     */
    virtual void update() override;

    char *get_name() override;

};



//class move_to_next_cross

inline move_command::move_command(boe_bot *robot_p, location final_location_p) : boe_bot_command_base(robot_p,
                                                                                                      final_location_p),
                                                                                 cross_encountered(false),
                                                                                 cross_corrected(false),
                                                                                 cross_encountered_time(0) {};

void move_command::set(boe_bot *robot_p, const location &final_location_p) {
    init(robot_p, final_location_p);
    cross_encountered = false;
    cross_corrected = false;
    cross_encountered_time = 0;
}

inline void move_command::go_straight() {
    if (robot->get_sensors().middle()) {
        robot->half_forward();
    }
    if (robot->get_sensors().second_right()) {
        robot->slightly_right();
        return;
    }
    if (robot->get_sensors().second_left()) {
        robot->slightly_left();
        return;
    }
    /* If the sensors are located so that the line is between them */
    this->robot->quarter_forward();
};

inline void move_command::encounter_cross() {
    /* Move must be at least a little bit long */
    if (millis() - move_started < 300) {
        go_straight();
    } else if ((robot->get_sensors().first_left() || robot->get_sensors().first_right())) {
        robot->led_on();
        cross_encountered = true;
        cross_encountered_time = millis();
    } else {
        go_straight();
    }
};

void move_command::do_sensors_correction_on_cross() {
    if (robot->get_sensors().first_left() && !robot->get_sensors().second_left()) {
        robot->in_place_left();
    } else if (robot->get_sensors().first_right() && !robot->get_sensors().second_right()) {
        robot->in_place_right();
    } else if (robot->get_sensors().left_part() || robot->get_sensors().right_part()) {
        robot->half_forward();
    }
    cross_corrected = true;
};

inline void move_command::do_wheels_corrections_on_cross() {
    //NOTE: constant time in ms, which seems to be appropriate
    if (millis() - cross_encountered_time < 375 ||
        robot->get_sensors().left_part() || robot->get_sensors().right_part()) {
        go_straight();
    } else {
        robot->led_off();
        robot->stop_smoothly();

        finish();
    }
};

inline void move_command::update() {
    /* The move is finished */
    if (is_done()) {
        robot->stop();
        return;
    }

    /* First call to this function */
    if (state == command_state::PREPARED) {
        move_started = millis();
        state = command_state::IN_PROCESS;
        robot->clear_last_move_encounters();
    }

    /* During the move, check if left and right path exists */
    robot->check_for_path_encounters();
    if (cross_encountered && cross_corrected) {
        do_wheels_corrections_on_cross();
    } else if (cross_encountered && !cross_corrected) {
        do_sensors_correction_on_cross();
    } else {
        encounter_cross();
    }
};

inline char *move_command::get_name() {
    return (char *) "move command";
}

#endif
