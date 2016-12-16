#ifndef turn_command_h_
#define turn_command_h_

#include "boe_bot_command_base.h"

class turn_command : public boe_bot_command_base
{
	bool left;
	bool leavingFirstLine;

public:
	turn_command(bool left_p, boe_bot* robot_p, location final_location_p);
	virtual void update(const time_elapsed& time_elapsed) override;
	virtual bool force_stop() override;
};



//class turn

inline turn_command::turn_command(bool left_p, boe_bot* robot_p, location final_location_p) : boe_bot_command_base(robot_p, final_location_p), left(left_p), leavingFirstLine(true) {};

//if planing is correct we dont have to take care of corners
void turn_command::update(const time_elapsed& time_elapsed)
{
	if (is_done())
		return;

	if (leavingFirstLine)
	{
		if (left)
			robot->in_place_left();
		else
			robot->in_place_right();

		if (!robot->middle())
			leavingFirstLine = false;
	}
	else
	{
		if (robot->middle() && (!robot->second_left() && !robot->second_right()))
		{
			robot->stop();

			if (!time_constrain || time_constrain >= time_elapsed)
				finish();
		}
	}
};


inline bool turn_command::force_stop()
{
	finish();
	return true;
};

#endif
