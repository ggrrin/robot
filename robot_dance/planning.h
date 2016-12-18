#ifndef planning_h_
#define planning_h_

#include "location.h"

typedef unsigned long time_t;


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

    virtual void update(const time_t &time_elapsed) = 0;

    virtual bool is_done() = 0;

    virtual bool force_stop() = 0;

    virtual void set_time_constrain(const time_t &time_constrain) = 0;

};


/**
 * Interface for planner, which is able to compute route from one location to another and stores all commands.
 */
class planner {
public:

    virtual ~planner() {}

    virtual bool prepare_route(const location &source, const location &target, bool moveFirstX,
                               const time_t &time_constrain_p) = 0;

    virtual command *get_next_command() = 0;

};

#endif