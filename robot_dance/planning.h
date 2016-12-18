#ifndef planning_h_
#define planning_h_

#include "location.h"

typedef unsigned long time_ttt;


/**
 * Interface for all commands robot can do.
 */
class command {
public:

    /**
     * Possible states of the command.
     */
    enum command_state {
        /**
         * Command was created, but did not start yet.
         */
        PREPARED,
        /**
         * Command is currently being executed.
         */
        IN_PROCESS,
        /**
         * Command is done, but waits for clock - time synchronization.
         */
        WAITING,
        /**
         * Command is done, time condition is fulfilled.
         */
        FINISHED
    };

    virtual ~command() {}

    virtual void update(const time_ttt &time_elapsed) = 0;

    virtual bool is_done() = 0;

    virtual bool force_stop() = 0;

    virtual void set_time_constrain(const time_ttt &time_constrain) = 0;

	virtual char* get_name() = 0;
};


/**
 * Interface for planner, which is able to compute route from one location to another and stores all commands.
 */
class planner {
public:

    virtual ~planner() {}

    virtual bool prepare_route(const location &source, const location &target, bool moveFirstX, const time_ttt &time_constrain_p) = 0;

    virtual command *get_next_command() = 0;

};

class command_parser
{
public:
	virtual ~command_parser(){}

	virtual bool fetch_next() = 0;
	virtual location get_initial_location() = 0;
	virtual position get_current_target() = 0;
	virtual bool is_first_directionX() = 0;
	virtual time_ttt get_finish_time_constraion() = 0;
};

#endif