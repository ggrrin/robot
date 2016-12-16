#ifndef move_command_h_
#define move_command_h_

#include "boe_bot_command_base.h"

class move_command : public boe_bot_command_base
{
	bool cross_encountered;
	time_elapsed cross_encountered_time;

	void go_straight(const time_elapsed& time_elapsed);
	void move_sensors_to_cross(const time_elapsed& time_elapsed);
	void move_wheels_to_cross(const time_elapsed& time_elapsed);

public:
	explicit move_command(boe_bot* robot_p, location final_location_p);
	virtual void update(const time_elapsed& time_elapsed) override;
	virtual bool force_stop() override;
};



//class move_to_next_cross

inline move_command::move_command(boe_bot* robot_p, location final_location_p) : boe_bot_command_base(robot_p, final_location_p) {};

inline void move_command::go_straight(const time_elapsed& time_elapsed)
{
	if (this->robot->second_right()) {
		this->robot->slightly_right();
		return;
	}
	if (this->robot->second_left()) {
		this->robot->slightly_left();
		return;
	}
	if (this->robot->middle()) {
		this->robot->half_forward();
		return;
	}
};

inline void move_command::move_sensors_to_cross(const time_elapsed& time_elapsed)
{
	//assuming command is first called when all sensors are not at line
	//TODO margin sensors"
	if (this->robot->second_right() && robot->second_left() && robot->middle())
	{
		cross_encountered = true;
		cross_encountered_time = time_elapsed;
		return;
	}
	else
	{
		go_straight(time_elapsed);
	}
};

inline void move_command::move_wheels_to_cross(const time_elapsed& time_elapsed)
{
	//TODO find appropriate constant or do in more advanced way
	if (time_elapsed - cross_encountered_time < 300)
	{
		go_straight(time_elapsed);
	}
	else
	{
		robot->stop();

		if (!time_constrain || time_constrain >= time_elapsed)
			finish();
	}
};

inline void move_command::update(const time_elapsed& time_elapsed)
{
	if (is_done())
		return;

	if (cross_encountered)
		move_wheels_to_cross(time_elapsed);
	else
		move_sensors_to_cross(time_elapsed);
};

inline bool move_command::force_stop()
{
	finish();
	return true;
};

#endif
