#ifndef planning_h_
#define planning_h_

#include "location.h"
#include "robot_dance.hpp"


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
         * Command is done, time condition is fulfilled.
         */
        FINISHED
    };

    virtual ~command() {}

    virtual void update() = 0;

    virtual bool is_done() = 0;

    /**
     * Gets printable name of the command.
     *
     * @return The printable name of the command.
     */
    virtual char *get_name() = 0;

};


/**
 * Interface for planner, which is able to compute route from one location to another and stores all commands.
 */
class planner {
public:

    virtual ~planner() {}

    virtual bool prepare_route(const location &source, const location &target, bool moveFirstX) = 0;

    virtual command *get_next_command() = 0;

};

/**
 * Interface for all dance-commands parsers.
 */
class command_parser {
public:
    virtual ~command_parser() {}

    /**
     * Prepares the initial location & direction.
     *
     * @return If the initial command exists.
     */
    virtual bool fetch_initial() = 0;

    /**
     * Prepares next command.
     *
     * @return If next command exists.
     */
    virtual bool fetch_next() = 0;

    /**
     * Gets the first/home/initial position.
     *
     * @return The first/home/initial position.
     */
    virtual location get_initial_location() = 0;

    /**
     * Gets the current target position.
     *
     * @return The current target position.
     */
    virtual position get_current_target() = 0;

    /**
     * Decides if the horizontal axis has higher priority.
     *
     * @return If the horizontal axis has higher priority.
     */
    virtual bool is_first_directionX() = 0;

    /**
     * Gets absolute time, when the current command should end.
     *
     * @return The absolute time, when the current command should end.
     */
    virtual time_type get_finish_time_constrain() = 0;

    /**
     * Tries to add and parse one new character from dance sequence.
     * Should return false if the sequence is incorrect.
     *
     * @param character Character to be parsed.
     * @return True if the sequence is without errors, false otherwise.
     */
    virtual bool store_character(const char &character) = 0;

    /**
     * Resets the parser to its initial state - back to the first command.
     */
    virtual void reset_commands() = 0;

};

#endif