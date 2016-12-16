#ifndef boe_bot_command_base_h_
#define boe_bot_command_base_h_

#include "planning.h"
#include "boe_bot.h"

class boe_bot_command_base : public command
{
	bool finished;
	location final_location;
protected:
	time_elapsed time_constrain;
	boe_bot* robot;
	boe_bot_command_base(boe_bot* robot_p, location final_location_p);

	void finish();


public:
	virtual ~boe_bot_command_base() {};
	virtual bool is_done() override;
	virtual void set_time_constrain(const time_elapsed& time_constrain) override;
};


//definition

//class boe_bot_command_base

inline boe_bot_command_base::boe_bot_command_base(boe_bot* robot_p, location final_location_p) : finished(false), final_location(final_location_p), time_constrain(0), robot(robot_p) {}

inline void boe_bot_command_base::finish()
{
	robot->set_location(final_location);
	finished = true;
};

inline bool boe_bot_command_base::is_done()
{
	return finished;
}

inline void boe_bot_command_base::set_time_constrain(const time_elapsed& time_constrain)
{
	this->time_constrain = time_constrain;
};

#endif