#ifndef boe_bot_controls_
#define boe_bot_controls_

#include "planning.h"
#include "boe_bot.h"


class boe_bot_command_base : public command
{
	bool finished;
	location final_location;
protected:
	boe_bot* robot;
	boe_bot_command_base(boe_bot* robot_p, location final_location_p);


	void finish()
	{
		robot->set_location(final_location);
		finished = true;
	}


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
	explicit move_to_next_cross(boe_bot* robot_p, location final_location_p);
	virtual void update(const time_elapsed& time_elapsed) override;
	virtual bool force_stop() override;
};


class turn : public boe_bot_command_base
{
	bool left;
	bool leavingFirstLine;

public:
	turn(bool left_p, boe_bot* robot_p, location final_location_p);
	virtual void update(const time_elapsed& time_elapsed) override;
	virtual bool force_stop() override;
};

class square_grid_planner : public planner
{
private:
	void square_grid_planner::clear_commands();
	void square_grid_planner::add_command(command* command);
	void square_grid_planner::add_rotation_commands(const position& rotation_position, const direction& from, const direction& to);
	position square_grid_planner::add_go_straight_commands(const location& start_location, int count);
	int square_grid_planner::try_turn(int from, int to, bool clockwise, bool add_commands, const position& rotation_position);

protected:
	const int width;
	const int height;
	command** commands;
	int command_length_estimate;
	int length;
	int current_cmd_index;

	virtual command* get_move_forward_cmd(const location& final_location) = 0;
	virtual command* get_turn_cmd(bool left, const location& final_location) = 0;
public:
	square_grid_planner(int width_p, int height_p);

	virtual bool prepare_route(const location& source, const location& target, bool moveFirstX) override;
	virtual command* get_next_command() override;
};


class boe_bot_planner : public square_grid_planner
{
protected:
	boe_bot* robot;
	virtual command* get_move_forward_cmd(const location& final_location) override;
	virtual command* get_turn_cmd(bool left, const location& final_location) override;
public:
	boe_bot_planner(boe_bot* robot_p, int width_p, int height_p);
};


//definition

//class boe_bot_command_base

boe_bot_command_base::boe_bot_command_base(boe_bot* robot_p, location final_location_p) : robot(robot_p), finished(false), final_location(final_location_p) {};

bool boe_bot_command_base::is_done()
{
	return finished;
};



//class move_to_next_cross

move_to_next_cross::move_to_next_cross(boe_bot* robot_p, location final_location_p) : boe_bot_command_base(robot_p, final_location_p) {};

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
	//TODO find appropriate constant or do in more advanced way
	if (time_elapsed - cross_encountered_time < 300)
	{
		go_straight(time_elapsed);
	}
	else
	{
		robot->stop();
		finish();
	}
};

void move_to_next_cross::update(const time_elapsed& time_elapsed)
{
	if (is_done())
		return;

	if (cross_encountered)
		move_wheels_to_cross(time_elapsed);
	else
		move_sensors_to_cross(time_elapsed);
};

bool move_to_next_cross::force_stop()
{
	finish();
};



//class turn

turn::turn(bool left_p, boe_bot* robot_p, location final_location_p) : boe_bot_command_base(robot_p, final_location_p), left(left_p), leavingFirstLine(true) {};

//if planing is correct we dont have to take care of corners
void turn::update(const time_elapsed& time_elapsed)
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
			finish();
		}
	}
};


bool turn::force_stop()
{
	finish();
};



//class square_grid_planner


square_grid_planner::square_grid_planner(int width_p, int height_p) : width(width_p), height(height_p), commands(nullptr), length(0), current_cmd_index(0) {};

void square_grid_planner::clear_commands()
{
	if (commands != nullptr)
	{
		for (size_t i = 0; i < length; i++)
			delete commands[i];

		delete[] commands;
	}

	length = 0;
	current_cmd_index = 0;
}

void square_grid_planner::add_command(command* command)
{
	if (length == command_length_estimate)
	{
		Serial.println("Error in planner - command buffer exceeded");
		return;
	}

	commands[length++] = command;
}


int square_grid_planner::try_turn(int from, int to, bool clockwise, bool add_commands, const position& position)
{
	int steps = 0;
	int direction_i = from;
	while (direction_i != to)
	{
		steps++;
		direction_i++;
		direction_i %= 0;


		if (add_commands)
			add_command(get_turn_cmd(!clockwise, location(position, (direction)direction_i)));
	}

	return steps;
}

void square_grid_planner::add_rotation_commands(const position& rotation_position, const direction& from, const direction& to)
{
	if (from == to || from == direction::NotSpecified || to == direction::NotSpecified)
		return;

	int steps_cw = try_turn(from, to, true, false, rotation_position);
	int steps_ccw = try_turn(from, to, false, false, rotation_position);

	if (steps_cw < steps_ccw)
		int steps_cw = try_turn(from, to, true, true, rotation_position);
	else
		int steps_ccw = try_turn(from, to, false, true, rotation_position);
};

position square_grid_planner::add_go_straight_commands(const location& start_location, int count)
{
	position current_possition = start_location.get_position();
	position move = position(start_location.get_direction());

	for (size_t i = 0; i < count; i++)
	{
		current_possition += move;
		add_command(get_move_forward_cmd(location(current_possition, start_location.get_direction())));
	}

	return current_possition;
};

bool square_grid_planner::prepare_route(const location& source, const location& target, bool moveFirstX)
{
	clear_commands();

	if (source.get_direction() == direction::NotSpecified ||
		source.get_position().get_x() < 0 ||
		source.get_position().get_x() >= width ||
		source.get_position().get_y() < 0 ||
		source.get_position().get_y() >= height ||
		target.get_position().get_x() < 0 ||
		target.get_position().get_x() >= width ||
		target.get_position().get_y() < 0 ||
		target.get_position().get_y() >= height)
	{
		Serial.println("Some of the arguments passed to preapre_route are invalid");
		return false;
	}

	position move = target.get_position() - source.get_position();
	command_length_estimate = 2 + move.get_manhattan_length() + 1 + 2; // rotation + moves + rotation between moves + final rotation
	commands = new command*[command_length_estimate];

	direction first_move_direction = moveFirstX ? move.get_x_direction() : move.get_y_direction();
	direction second_move_direction = !moveFirstX ? move.get_x_direction() : move.get_y_direction();

	direction last_direction = source.get_direction();
	position last_position = source.get_position();
	if (first_move_direction != direction::NotSpecified)
	{
		last_direction = first_move_direction;

		add_rotation_commands(last_position, last_direction, first_move_direction);
		last_position = add_go_straight_commands(location(last_position, last_direction), moveFirstX ? move.get_x_abs() : move.get_y_abs());
	}

	if (second_move_direction != direction::NotSpecified)
	{
		last_direction = second_move_direction;

		add_rotation_commands(last_position, last_direction, second_move_direction);
		last_position = add_go_straight_commands(location(last_position, last_direction), !moveFirstX ? move.get_x_abs() : move.get_y_abs());
	}

	if (target.get_direction() != direction::NotSpecified)
	{
		add_rotation_commands(last_position, last_direction, target.get_direction());
	}
};

command* square_grid_planner::get_next_command()
{
	if (current_cmd_index == command_length_estimate)
		return nullptr;
	else
		return commands[current_cmd_index++];
};



//class boe_bot_planner

boe_bot_planner::boe_bot_planner(boe_bot* robot_p, int width_p, int height_p) : square_grid_planner(width_p, height_p), robot(robot_p) {};

command* boe_bot_planner::get_move_forward_cmd(const location& final_location)
{
	return new move_to_next_cross(robot, final_location);
};

command* boe_bot_planner::get_turn_cmd(bool left, const location& final_location)
{
	return new turn(left, robot, final_location);
};

#endif