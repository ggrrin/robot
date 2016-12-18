#ifndef command_parser_mocap_h
#define command_parser_mocap_h

#include "planning.h"

#define MOVE
//#define TURNL 
//#define TURNR
//#define CMP
class command_parser_mocap : public command_parser
{
#ifdef MOVE
	position pos[10] = {
		position(1,0),
	};

	bool firstX[10] = {
		false,
	};

	time_ttt times[10]{
		0,
	};

	location initial_location  = location(0,0, direction::East);

#endif

#ifdef  TURNL
	position pos[10] = {
		position(4,3),
	};

	bool firstX[10] = {
		false,
	};

	time_ttt times[10]{
		0,
	};

	location initial_location  = location(3,3, direction::South);
#endif

#ifdef TURNR
	position pos[10] = {
		position(2,3),
	};

	bool firstX[10] = {
		true,
	};

	time_ttt times[10]{
		0,
	};

	location initial_location  = location(3,3, direction::South);
#endif

#ifdef CMP

	position pos[10] = {
		position(0,0),
		position(2,3),
		position(1,1),
		position(0,0),
	};

	bool firstX[10] = {
		false,
		true,
		true,
		false
	};

	time_ttt times[10]{
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
	virtual time_ttt get_finish_time_constraion() override;
};

inline command_parser_mocap::command_parser_mocap(): current_instruction(-1)
{
}

inline bool command_parser_mocap::fetch_next()
{
	current_instruction++;
	return current_instruction < sizeof(pos);
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

inline time_ttt command_parser_mocap::get_finish_time_constraion()
{
	return times[current_instruction];
}

#endif
