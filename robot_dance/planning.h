#ifndef planning_h_
#define planning_h_

#include "location.h"

typedef unsigned long time_elapsed;

class command
{
public:
	virtual ~command() { }

	virtual void update(const time_elapsed& time_elapsed) = 0;
	virtual bool is_done() = 0;
	virtual bool force_stop() = 0;
	virtual void set_time_constrain(const time_elapsed& time_constrain) = 0;
};


class planner
{
public:
	virtual ~planner() { }

	virtual bool prepare_route(const location& source, const location& target, bool moveFirstX, const time_elapsed& time_constrain_p) = 0;
	virtual command* get_next_command() = 0;
};

#endif