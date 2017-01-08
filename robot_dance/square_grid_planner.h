#ifndef square_grid_planner_h_
#define square_grid_planner_h_

#include "planning.h"


/**
 * Planner, which is able to make routes on discrete plane - grid map.
 */
class square_grid_planner : public planner {
private:

    /**
     * Removes all commands.
     */
    void clear_commands();

    /**
     * Adds given command to the queue if there is space in the queue.
     *
     * @param command New command to be queued.
     * @return If the command could be added to the queue.
     */
    bool add_command(command *command, const location &result_location);

    /**
     * Adds rotation commands to the queue so that from given direction to desired position
     * minimal number steps is performed.
     *
     * @param rotation_position Original position.
     * @param from Original direction.
     * @param to Desired direction.
     */
    void add_rotation_commands(const position &rotation_position, const direction &from, const direction &to);

    /**
     * Adds given number of move commands to the queue.
     *
     * @param start_location Initial location.
     * @param count Number of tiles to move through.
     * @return Target position counted from given information.
     */
    position add_go_straight_commands(const location &start_location, int count);

    /**
     * Counts number of rotation steps for given initial and desired situation. Adds commands
     * to the queue if desired.
     *
     * @param from Initial direction.
     * @param to Desired direction.
     * @param clockwise Desired rotation direction.
     * @param add_commands Defines if appropriate commands should be added to the queue.
     * @param rotation_position Position of the rotation.
     * @return Number of steps to get to desired direction with given requirements.
     */
    int try_turn(const direction &from, const direction &to, bool clockwise, bool add_commands,
                 const position &rotation_position);

protected:

    /**
     * Next command to be executed.
     */
    command *next_command = nullptr;

    /**
     * Current position & direction of the robot.
     */
    location current_location;

    /**
     * Target position & direction of the robot.
     */
    location target_location;

    /**
     * Defines if the robot should first go along the X(horizontal) or Y(vertical) axis.
     */
    bool move_first_X;

    virtual command *get_move_forward_cmd(const location &final_location) = 0;

    virtual command *get_turn_cmd(bool left, const location &final_location) = 0;

    virtual void prepare_route_step(const location &source, const location &target, bool moveFirstX);

public:

    virtual ~square_grid_planner();

    /**
     * Creates a planner, which is able to make routes on a grid map.
     */
    square_grid_planner();

    /**
     * Counts the route from given source to target location with specified axis priority.
     *
     * @param source Initial location.
     * @param target Desired target location.
     * @param moveFirstX Defines axis priority.
     * @return returns false on failure otherwise true
     */
    virtual bool prepare_route(const location &source, const location &target, bool moveFirstX) override;

    /**
     * Gets next command from the queue or nullptr if queue is empty.
     *
     * @return The next command from the queue or nullptr if queue is empty.
     */
    virtual command *get_next_command() override;

};



//class square_grid_planner

inline void square_grid_planner::prepare_route_step(const location &source, const location &target, bool moveFirstX) {
    clear_commands();

    position move = target.get_position() - source.get_position();

    direction first_move_direction = moveFirstX ? move.get_x_direction() : move.get_y_direction();
    direction second_move_direction = !moveFirstX ? move.get_x_direction() : move.get_y_direction();

    direction last_direction = source.get_direction();
    position last_position = source.get_position();

    if (first_move_direction != direction::NotSpecified) {
        add_rotation_commands(last_position, last_direction, first_move_direction);
        last_direction = first_move_direction;

        last_position = add_go_straight_commands(location(last_position, last_direction),
                                                 moveFirstX ? move.get_x_abs() : move.get_y_abs());
    }

    if (second_move_direction != direction::NotSpecified) {
        add_rotation_commands(last_position, last_direction, second_move_direction);
        last_direction = second_move_direction;

        last_position = add_go_straight_commands(location(last_position, last_direction),
                                                 !moveFirstX ? move.get_x_abs() : move.get_y_abs());
    }

    if (target.get_direction() != direction::NotSpecified) {
        add_rotation_commands(last_position, last_direction, target.get_direction());
    }
}

inline square_grid_planner::~square_grid_planner() {
    clear_commands();
};

inline square_grid_planner::square_grid_planner() : move_first_X(false) {};

inline void square_grid_planner::clear_commands() {
    next_command = nullptr;
}

inline bool square_grid_planner::add_command(command *command, const location &result_location) {
    if (next_command != nullptr) {
        return false;
    }

    next_command = command;
    current_location = result_location;

    return true;
}

inline int square_grid_planner::try_turn(const direction &from, const direction &to, bool clockwise, bool add_commands,
                                         const position &rotation_position) {
    int steps = 0;
    direction direction_i = from;
    while (direction_i != to) {
        steps++;
        switch (direction_i) {
            case direction::North:
                direction_i = clockwise ? direction::East : direction::West;
                break;
            case direction::East:
                direction_i = clockwise ? direction::South : direction::North;
                break;
            case direction::South:
                direction_i = clockwise ? direction::West : direction::East;
                break;
            case direction::West:
                direction_i = clockwise ? direction::North : direction::South;
                break;
            case NotSpecified:
                return 0;
        }

        if (add_commands) {
            location loc(rotation_position, direction_i);
            if (next_command == nullptr) {
                add_command(get_turn_cmd(!clockwise, loc), loc);
            }
            break;
        }
    }

    return steps;
}

inline void square_grid_planner::add_rotation_commands(const position &rotation_position, const direction &from,
                                                       const direction &to) {
    if (from == to || from == direction::NotSpecified || to == direction::NotSpecified) {
        return;
    }

    int steps_cw = try_turn(from, to, true, false, rotation_position);
    int steps_ccw = try_turn(from, to, false, false, rotation_position);

    try_turn(from, to, steps_cw < steps_ccw, true, rotation_position);
};

inline position square_grid_planner::add_go_straight_commands(const location &start_location, int count) {
    position current_position = start_location.get_position();
    position move = position(start_location.get_direction());

    current_position += move;
    location loc(current_position, start_location.get_direction());
    if (next_command == nullptr) {
        add_command(get_move_forward_cmd(loc), loc);
    }

    return current_position;
};

inline bool square_grid_planner::prepare_route(const location &source, const location &target, bool moveFirstX) {
    current_location = source;
    target_location = target;
    move_first_X = moveFirstX;
    return true;
};

inline command *square_grid_planner::get_next_command() {
    prepare_route_step(current_location, target_location, move_first_X);
    return next_command;
};

#endif
