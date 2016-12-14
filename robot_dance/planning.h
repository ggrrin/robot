#ifndef planning_
#define planning_

#include "location.h"

typedef unsigned long time_elapsed;

class command
{
public:
	virtual void update(const time_elapsed& time_elapsed) = 0;
	virtual bool is_done() = 0;
	virtual bool force_stop() = 0;
};


class planner
{
public:
	virtual bool prepare_route(const location& source, const location& target, bool moveFirstX) = 0;
	virtual command* get_next_command() = 0;
};

#endif