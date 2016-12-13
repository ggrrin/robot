#ifndef boe_bot_controls_
#define boe_bot_controls_

#include "planning.h"
#include "boe_bot.h"


class boe_bot_command_base : public command
{
protected:
	boe_bot* robot;
	bool finished;
	boe_bot_command_base(boe_bot* robot_p);
	
public:
	virtual bool is_done() override;
};


class move_to_next_cross : public boe_bot_command_base 
{
	bool cross_encountered;
	time_elapsed cross_encountered_time;

	void go_straight(const time_elapsed& time_elapsed);
	void move_sensors_to_cross(const time_elapsed& time_elapsed);
	void move_wheels_to_cross(const time_elapsed& time_elapsed);

public:
	explicit move_to_next_cross(boe_bot* robot_p);
	virtual void update(const time_elapsed& time_elapsed) override;
	virtual bool force_stop() override;
};

class turn : public boe_bot_command_base 
{
	bool left;

public:
	turn(bool left_p, boe_bot* robot_p);
	virtual void update(const time_elapsed& time_elapsed) override;
	virtual bool force_stop() override;
};

class boe_bot_planner : public planner
{
	boe_bot* robot;
	int width;
	int height;
public:
	boe_bot_planner(boe_bot* robot_p, int width_p, int height_p);

	virtual void prepare_route(const location& source, const location& target) override;
	virtual command* get_next_command() override;
};


//definition

//class boe_bot_command_base

boe_bot_command_base::boe_bot_command_base(boe_bot* robot_p) : robot(robot_p), finished(false) {};

bool boe_bot_command_base::is_done()
{
	return finished;
};



//class move_to_next_cross

move_to_next_cross::move_to_next_cross(boe_bot* robot_p) : boe_bot_command_base(robot_p) {};

void move_to_next_cross::go_straight(const time_elapsed& time_elapsed)
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

void move_to_next_cross::move_sensors_to_cross(const time_elapsed& time_elapsed)
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

void move_to_next_cross::move_wheels_to_cross(const time_elapsed& time_elapsed)
{
	//TODO find appropriate constant or do more advanced way
	if (time_elapsed - cross_encountered_time < 300)
	{
		go_straight(time_elapsed);
	}
	else
	{
		robot->stop();
		this->finished = true;
	}
};

void move_to_next_cross::update(const time_elapsed& time_elapsed)
{
	if (finished)
		return;

	if (cross_encountered)
		move_wheels_to_cross(time_elapsed);
	else
		move_sensors_to_cross(time_elapsed);
};

bool move_to_next_cross::force_stop()
{
	finished = true;
};



//class turn

turn::turn(bool left_p, boe_bot* robot_p) : boe_bot_command_base(robot_p),   left(left_p) {};

void turn::update(const time_elapsed& time_elapsed)
{
	if (finished)
		return;

	//TODO turning
};


bool turn::force_stop()
{
	finished = true;
};



//class boe_bot_planner

boe_bot_planner::boe_bot_planner(boe_bot* robot_p, int width_p, int height_p) :robot(robot_p), width(width_p), height(height_p) {};

void boe_bot_planner::prepare_route(const location& source, const location& target)
{
	//TODO
};

command* boe_bot_planner::get_next_command()
{
	//TODO
};

#endif