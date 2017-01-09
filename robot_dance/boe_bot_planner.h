#ifndef boe_bot_planer_h_
#define boe_bot_planer_h_

#include "boe_bot.hpp"
#include "square_grid_planner.h"
#include "move_command.h"
#include "turn_command.h"


/**
 * Implementation of the grid planner, which is able to prepare commands
 * for routes making the robot go anywhere on the grid map.
 */
class boe_bot_planner : public square_grid_planner {
private:

    /**
     * Stores one move command locally to avoid dynamic allocation.
     */
    move_command _move_command;

    /**
     * Stores one turn command locally to avoid dynamic allocation.
     */
    turn_command _turn_command;

protected:

    /**
     * Robot to take commands.
     */
    boe_bot *robot;

    /**
     * Creates a new command to move the robot to given location one tile away.
     *
     * @param final_location Desired final location of the robot.
     * @return Command able to perform move task.
     */
    virtual command *get_move_forward_cmd(const location &final_location) override;

    /**
     * Creates a new command to turn the robot for 90deg into given location with specified direction.
     *
     * @param left Direction of the rotation.
     * @param final_location Desired final location of the robot.
     * @return Command able to perform rotation task.
     */
    virtual command *get_turn_cmd(bool left, const location &final_location) override;

public:

    /**
     * Create a new planner capable to make plans for given grid size.
     *
     * @param robot_p Robot to take commands.
     */
    boe_bot_planner(boe_bot *robot_p);
    boe_bot_planner();

};



//class boe_bot_planner

inline boe_bot_planner::boe_bot_planner(boe_bot *robot_p) : _move_command(robot_p, location()),
                                                            _turn_command(false, robot_p, location()),
                                                            square_grid_planner(), robot(robot_p) {}

inline boe_bot_planner::boe_bot_planner() : _move_command(nullptr, location()),
_turn_command(false, nullptr, location()),
square_grid_planner(), robot(nullptr) {};

inline command *boe_bot_planner::get_move_forward_cmd(const location &final_location) {
    _move_command.set(robot, final_location);
    return &_move_command;
};

inline command *boe_bot_planner::get_turn_cmd(bool left, const location &final_location) {
    _turn_command.set(left, robot, final_location);
    return &_turn_command;
};

#endif