#ifndef boe_bot_command_base_h_
#define boe_bot_command_base_h_

#include "planning.h"
#include "boe_bot.hpp"


/**
 * Abstract class with partial implementation of the 'command' interface.
 */
class boe_bot_command_base : public command {

private:
    /**
     * Desired final position and direction of the robot.
     */
    location final_location;

protected:

    /**
     * Robot performing the command tasks.
     */
    boe_bot *robot;

    /**
     * Specifies current state of the command.
     */
    command_state state;

    /**
     * Create a new command for given robot to move to given final location.
     *
     * @param robot_p Robot to be commanded.
     * @param final_location_p Desired final position & rotation of the robot.
     */
    boe_bot_command_base(boe_bot *robot_p, location final_location_p);

    /**
     * Alternative 'constructor' to avoid dynamic allocation.
     *
     * @param robot_p Robot to be commanded.
     * @param final_location_p Desired final position & rotation of the robot.
     */
    void init(boe_bot *robot_p, location final_location_p);

    /**
     * Mark this command as completed, robot's position is moved to desired location.
     */
    void finish();

public:

    virtual ~boe_bot_command_base() {};

    virtual bool is_done() override;

    /**
     * Gets human-readable string representation of this command.
     *
     * @return The human-readable string representation of this command.
     */
    virtual char *get_name() {
        return (char *) "boe_bot_command_base command";
    };

};



//class boe_bot_command_base

inline boe_bot_command_base::boe_bot_command_base(boe_bot *robot_p, location final_location_p) : state(PREPARED),
                                                                                                 final_location(
                                                                                                         final_location_p),
                                                                                                 robot(robot_p) {}

void boe_bot_command_base::init(boe_bot *robot_p, location final_location_p) {
    state = PREPARED;
    final_location = final_location_p;
    robot = robot_p;
}

inline void boe_bot_command_base::finish() {
    robot->set_location(final_location);
    state = FINISHED;
};

inline bool boe_bot_command_base::is_done() {
    return state == FINISHED;
}

#endif