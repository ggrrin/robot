#pragma once

#include "../planning.h"
#include "../square_grid_planner.h"
#include <ostream>

class context
{
	location loc;

public:
	void set_loc(const location& loc)
	{
		this->loc = loc;
	}

	explicit context(const location& initial_location)
		: loc(initial_location)
	{
	}

	location get_location() const
	{
		return loc;
	}

	friend std::ostream& operator<<(std::ostream& os, const context& obj)
	{
		char* dir_str = nullptr;
		switch (obj.loc.get_direction())
		{
		case NotSpecified:
			dir_str = "NotSpecified";
			break;
		case North:
			dir_str = "North";
			break;
		case East:
			dir_str = "East";
			break;
		case South:
			dir_str = "South";
			break;
		case West:
			dir_str = "West";
			break;
		default:
			dir_str = "Probably error in enum !!!";
			break;
		}


		return os << "loc: {" << obj.loc.get_position().get_x() << ", " << obj.loc.get_position().get_y() << "} " << dir_str << std::endl;
	}
};


class turn_command_mocap : public command
{
	context* ctx;
	bool done = false;
	bool left;
	location final_location;
public:
	explicit turn_command_mocap(bool left, context* ctx, location final_location)
		: ctx(ctx), left(left), final_location(final_location)
	{
	}

	void update(const time_type& time_elapsed) override
	{
		//auto dir = ctx->get_location().get_direction() + (left ? -1 : +1);
		//dir %= 4;
		//ctx->set_loc(location(ctx->get_location().get_position(), static_cast<direction>(dir)));

		ctx->set_loc(final_location);

		done = true;
	}

	bool is_done() override
	{
		return done;
	}

    void set_time_constrain(const time_type& time_constrain) override
	{

	}

	char* get_name() override
	{
		if (left)
			return "turn [left]";
		else
			return "turn [right]";
	}
};


class move_command_mocap : public command
{
	context* ctx;
	bool done = false;
	location final_location;
public:
	explicit move_command_mocap(context* ctx, location final_location)
		: ctx(ctx), final_location(final_location)
	{ }


	void update(const time_type& time_elapsed) override
	{
		//auto pos = ctx->get_location().get_position();
		//auto dir = ctx->get_location().get_direction();

		//switch (dir)
		//{
		//case North: 
		//	pos += position(0, 1);
		//	break;

		//case East: 
		//	pos += position(0, 1);
		//	break;

		//case South: 
		//	pos += position(0, -1);
		//	break;

		//case West: 
		//	pos += position(-1, 0);
		//	break;

		//case NotSpecified: break;
		//default: break;
		//}
		//ctx->set_loc(location(pos, dir));


		ctx->set_loc(final_location);
		done = true;
	}

	bool is_done() override
	{
		return done;
	}

    void set_time_constrain(const time_type& time_constrain) override
	{

	}

	char* get_name() override
	{
		return "move";
	}
};

class test_planner : public square_grid_planner
{
	context* ctx;

protected:
	command* get_move_forward_cmd(const location& final_location) override;
	command* get_turn_cmd(bool left, const location& final_location) override;

public:
	test_planner(context* ctx, int width_p, int height_p)
		: square_grid_planner(width_p, height_p), ctx(ctx)
	{
	}

};

inline command* test_planner::get_move_forward_cmd(const location& final_location)
{
	return new move_command_mocap(ctx, final_location);
}

inline command* test_planner::get_turn_cmd(bool left, const location& final_location)
{
	return new turn_command_mocap(left, ctx, final_location);
}
