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
     * Time in which the destination cross was reached measured in absolute time.
     */
    time_type cross_encountered_time;

    /**
     * Move the robot so that it follows the line.
     *
     * @param time_elapsed Absolute time from the beginning of dance.
     */
    void go_straight(const time_type &time_elapsed);

    /**
     * Moves straight until it finds a cross (or partial cross = corner).
     *
     * @param time_elapsed Absolute time from the beginning of dance.
     */
    void move_sensors_to_cross(const time_type &time_elapsed);

    /**
     * Move the robot forwards until the sensors are out of the cross.
     * Makes the robot centered on the cross.
     *
     * @param time_elapsed Absolute time from the beginning of dance.
     */
    void move_wheels_to_cross(const time_type &time_elapsed);

public:

    /**
     * Creates a new command to move the robot one tile away from current position.
     *
     * @param robot_p Robot to be commanded.
     * @param final_location_p Desired final position of the robot.
     */
    explicit move_command(boe_bot *robot_p, location final_location_p);

    /**
     * Continue to do this command.
     *
     * @param time_elapsed Absolute elapsed time from the start of the dance.
     */
    virtual void update(const time_type &time_elapsed) override;

    /**
     * Force this command to stop regardless of its state.
     *
     * @return //TODO ?
     */
    virtual bool force_stop() override;
	char* get_name() override;
};



//class move_to_next_cross

inline move_command::move_command(boe_bot *robot_p, location final_location_p) : boe_bot_command_base(robot_p,
                                                                                                      final_location_p) {};

inline void move_command::go_straight(const time_type &time_elapsed) {
    /* If on cross, move slowly */
    if (this->robot->get_sensors().middle()) {
        this->robot->quarter_forward();
        return;
    }
    if (this->robot->get_sensors().second_right()) {
        this->robot->slightly_right();
        return;
    }
    if (this->robot->get_sensors().second_left()) {
        this->robot->slightly_left();
        return;
    }
    if (this->robot->get_sensors().middle()) {
        this->robot->half_forward();
        return;
    }
};

inline void move_command::move_sensors_to_cross(const time_type &time_elapsed) {
    //assuming command is first called when all sensors are not at line
    //NOTE: this should work even in the corners of the grid
    if ((robot->get_sensors().left_part() || robot->get_sensors().right_part()) && robot->get_sensors().middle()) {
        cross_encountered = true;
        cross_encountered_time = time_elapsed;

        return;
    } else {
        go_straight(time_elapsed);
    }

};

inline void move_command::move_wheels_to_cross(const time_type &time_elapsed) {
//    //TODO find appropriate constant or do in more advanced way
//    if (time_elapsed - cross_encountered_time < 300) {
    /* Go straight until you get sensors out of the cross */
    if ((robot->get_sensors().left_part() || robot->get_sensors().right_part()) && robot->get_sensors().middle()) {
        go_straight(time_elapsed);
    } else {
        robot->stop();

        state = command_state::WAITING;

        if (!time_constrain || time_constrain >= time_elapsed) {
            finish();
        }
    }
};

inline void move_command::update(const time_type &time_elapsed) {
    if (is_done()) {
        return;
    }

    /* First call to this function */
    if (state == command_state::PREPARED) {
        state = command_state::IN_PROCESS;
    }

    if (cross_encountered)
        move_wheels_to_cross(time_elapsed);
    else
        move_sensors_to_cross(time_elapsed);
};


inline bool move_command::force_stop() {
	finish();
	//TODO co vrací? 
	//taky mi to ted nedava smysl :D ... ono se to snad vubec nebude pouzivat

	return true;
}


inline char* move_command::get_name()
{
	return "move command";

};

#endif
