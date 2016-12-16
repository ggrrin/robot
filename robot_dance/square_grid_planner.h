#ifndef square_grid_planner_h_
#define square_grid_planner_h_

#include "planning.h"

class square_grid_planner : public planner
{
private:
	void clear_commands();
	void add_command(command* command);
	void add_rotation_commands(const position& rotation_position, const direction& from, const direction& to);
	position add_go_straight_commands(const location& start_location, int count);
	int try_turn(int from, int to, bool clockwise, bool add_commands, const position& rotation_position);

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

	virtual bool prepare_route(const location& source, const location& target, bool moveFirstX, const time_elapsed& time_constrain_p) override;
	virtual command* get_next_command() override;
};



//class square_grid_planner


inline square_grid_planner::square_grid_planner(int width_p, int height_p) : width(width_p), height(height_p), commands(nullptr), command_length_estimate(0), length(0), current_cmd_index(0) {};

inline void square_grid_planner::clear_commands()
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

inline void square_grid_planner::add_command(command* command)
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
		direction_i %= 4;


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

bool square_grid_planner::prepare_route(const location& source, const location& target, bool moveFirstX, const time_elapsed& time_constrain_p)
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

	if (length > 0)
		commands[length - 1]->set_time_constrain(time_constrain_p);
};

inline command* square_grid_planner::get_next_command()
{
	if (current_cmd_index == command_length_estimate)
		return nullptr;
	else
		return commands[current_cmd_index++];
};

#endif
