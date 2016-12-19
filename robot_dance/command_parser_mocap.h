#ifndef command_parser_mocap_h
#define command_parser_mocap_h

#include "planning.h"

#define MOVE
//#define TURNL 
//#define TURNR
//#define CMP
class command_parser_mocap : public command_parser
{

	virtual bool fetch_initial() override {};
	virtual bool store_character(const char &character) {};
	virtual void reset_commands() {};

#ifdef MOVE
	int size = 3;
	position pos[3] = {
		position(1,0),
		position(2,0),
		position(3,0),
	};

	bool firstX[3] = {
		true,
		true,
		true,
	};

	time_type times[10]{
		0,
		0,
		0,
	};

	location initial_location  = location(0,0, direction::East);
	

#endif

#ifdef  TURNL
	int size = 1;
	position pos[1] = {
		position(4,3),
	};

	bool firstX[10] = {
		false,
	};

	time_type times[10]{
		0,
	};

	location initial_location  = location(3,3, direction::South);
#endif

#ifdef TURNR
	int size = 1;
	position pos[1] = {
		position(2,3),
	};

	bool firstX[10] = {
		true,
	};

	time_type times[10]{
		0,
	};

	location initial_location  = location(3,3, direction::South);
#endif

#ifdef CMP

	int size = 4;
	position pos[4] = {
		position(2,3),
		position(1,1),
		position(0,0),
		position(3,3)
	};

	bool firstX[10] = {
		false,
		true,
		true,
		false
	};

	time_type times[10]{
		0,
		0,
		0,
		0,
	};

	location initial_location  = location(0,0, direction::West);
#endif

	int current_instruction;

public:
	explicit command_parser_mocap();

	virtual bool fetch_next() override;
	virtual location get_initial_location() override;
	virtual position get_current_target() override;
	virtual bool is_first_directionX() override;
	virtual time_type get_finish_time_constrain() override;
};

inline command_parser_mocap::command_parser_mocap(): current_instruction(-1)
{
}

inline bool command_parser_mocap::fetch_next()
{
	current_instruction++;
	bool res = current_instruction < size;
	return res;
}

inline location command_parser_mocap::get_initial_location()
{
	return initial_location;
}

inline position command_parser_mocap::get_current_target()
{
	return pos[current_instruction];
}

inline bool command_parser_mocap::is_first_directionX()
{
	return firstX[current_instruction];
}

inline time_type command_parser_mocap::get_finish_time_constrain()
{
	return times[current_instruction];
}

#endif
