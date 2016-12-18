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
     * @param width_p Width of the grid, number of columns.
     * @param height_p Height of the grid, number of rows.
     */
    boe_bot_planner(boe_bot *robot_p, int width_p, int height_p);

};



//class boe_bot_planner

inline boe_bot_planner::boe_bot_planner(boe_bot *robot_p, int width_p, int height_p) : square_grid_planner(width_p,
                                                                                                           height_p),
                                                                                       robot(robot_p) {};

inline command *boe_bot_planner::get_move_forward_cmd(const location &final_location) {
    return new move_command(robot, final_location);
};

inline command *boe_bot_planner::get_turn_cmd(bool left, const location &final_location) {
    return new turn_command(left, robot, final_location);
};

#endif