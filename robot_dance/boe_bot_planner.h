#ifndef boe_bot_planer_h_
#define boe_bot_planer_h_

#include "boe_bot.h"
#include "square_grid_planner.h"
#include "move_command.h"
#include "turn_command.h"

class boe_bot_planner : public square_grid_planner
{
protected:
	boe_bot* robot;
	virtual command* get_move_forward_cmd(const location& final_location) override;
	virtual command* get_turn_cmd(bool left, const location& final_location) override;
public:
	boe_bot_planner(boe_bot* robot_p, int width_p, int height_p);
};






//class boe_bot_planner

inline boe_bot_planner::boe_bot_planner(boe_bot* robot_p, int width_p, int height_p) : square_grid_planner(width_p, height_p), robot(robot_p) {};

inline command* boe_bot_planner::get_move_forward_cmd(const location& final_location)
{
	return new move_command(robot, final_location);
};

inline command* boe_bot_planner::get_turn_cmd(bool left, const location& final_location)
{
	return new turn_command(left, robot, final_location);
};

#endif