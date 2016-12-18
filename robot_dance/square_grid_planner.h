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
     */
    void add_command(command *command);

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
     * Width of the grid map - number of columns.
     */
    const int width;

    /**
     * Height of the grid map - number of rows.
     */
    const int height;

    /**
     * Commands queue.
     */
    command **commands;

    /**
     * Maximum size of the command queue.
     */
    int command_length_estimate;

    /**
     * Current number of stored commands.
     */
    int length;

    /**
     * Index to currently executed command from the queue.
     */
    int current_cmd_index;

    virtual command *get_move_forward_cmd(const location &final_location) = 0;

    virtual command *get_turn_cmd(bool left, const location &final_location) = 0;

public:

    /**
     * Creates a planner, which is able to make routes on a grid map.
     *
     * @param width_p Width of the grid map.
     * @param height_p Height of the grid map.
     */
    square_grid_planner(int width_p, int height_p);

    /**
     * Counts the route from given source to target location with specified axis priority.
     *
     * @param source Initial location.
     * @param target Desired target location.
     * @param moveFirstX Defines axis priority.
     * @param time_constrain_p Maximum absolute time for this command.
     * @return //TODO: co vracet?
     */
    virtual bool prepare_route(const location &source, const location &target, bool moveFirstX,
                               const time_t &time_constrain_p) override;

    /**
     * Gets next command from the queue or nullptr if queue is empty.
     *
     * @return The next command from the queue or nullptr if queue is empty.
     */
    virtual command *get_next_command() override;
};



//class square_grid_planner

inline square_grid_planner::square_grid_planner(int width_p, int height_p) : width(width_p), height(height_p),
                                                                             commands(nullptr),
                                                                             command_length_estimate(0), length(0),
                                                                             current_cmd_index(0) {};

inline void square_grid_planner::clear_commands() {
    if (commands != nullptr) {
        for (size_t i = 0; i < length; i++)
            delete commands[i];

        delete[] commands;
        commands = nullptr;
    }

    length = 0;
    current_cmd_index = 0;
}

inline void square_grid_planner::add_command(command *command) {
    if (length == command_length_estimate) {
        Serial.println("Error in planner - command buffer exceeded");
        return;
    }

    commands[length++] = command;
}

int square_grid_planner::try_turn(const direction &from, const direction &to, bool clockwise, bool add_commands,
                                  const position &position) {
    int steps = 0;
    direction direction_i = from;
    while (direction_i != to) {
        steps++;
        direction_i++;
        direction_i %= 4;

        if (add_commands)
            add_command(get_turn_cmd(!clockwise, location(position, direction_i)));
    }

    return steps;
}

void square_grid_planner::add_rotation_commands(const position &rotation_position, const direction &from,
                                                const direction &to) {
    if (from == to || from == direction::NotSpecified || to == direction::NotSpecified) {
        return;
    }

    int steps_cw = try_turn(from, to, true, false, rotation_position);
    int steps_ccw = try_turn(from, to, false, false, rotation_position);

    try_turn(from, to, steps_cw < steps_ccw, true, rotation_position);
};

position square_grid_planner::add_go_straight_commands(const location &start_location, int count) {
    position current_position = start_location.get_position();
    position move = position(start_location.get_direction());

    for (size_t i = 0; i < count; i++) {
        current_position += move;
        add_command(get_move_forward_cmd(location(current_position, start_location.get_direction())));
    }

    return current_position;
};

bool square_grid_planner::prepare_route(const location &source, const location &target, bool moveFirstX,
                                        const time_t &time_constrain_p) {
    clear_commands();

    if (source.get_direction() == direction::NotSpecified
        || source.get_position().get_x() < 0
        || source.get_position().get_x() >= width
        || source.get_position().get_y() < 0
        || source.get_position().get_y() >= height
        || target.get_position().get_x() < 0
        || target.get_position().get_x() >= width
        || target.get_position().get_y() < 0
        || target.get_position().get_y() >= height) {

        Serial.println("Some of the arguments passed to prepare_route are invalid");
        return false;
    }

    position move = target.get_position() - source.get_position();
    command_length_estimate =
            2 + move.get_manhattan_length() + 1 + 2; // rotation + moves + rotation between moves + final rotation
    commands = new command *[command_length_estimate];

    direction first_move_direction = moveFirstX ? move.get_x_direction() : move.get_y_direction();
    direction second_move_direction = !moveFirstX ? move.get_x_direction() : move.get_y_direction();

    direction last_direction = source.get_direction();
    position last_position = source.get_position();
    if (first_move_direction != direction::NotSpecified) {
        last_direction = first_move_direction;

        add_rotation_commands(last_position, last_direction, first_move_direction);
        last_position = add_go_straight_commands(location(last_position, last_direction),
                                                 moveFirstX ? move.get_x_abs() : move.get_y_abs());
    }

    if (second_move_direction != direction::NotSpecified) {
        last_direction = second_move_direction;

        add_rotation_commands(last_position, last_direction, second_move_direction);
        last_position = add_go_straight_commands(location(last_position, last_direction),
                                                 !moveFirstX ? move.get_x_abs() : move.get_y_abs());
    }

    /* Rotation on the final position */
    if (target.get_direction() != direction::NotSpecified) {
        add_rotation_commands(last_position, last_direction, target.get_direction());
    }

    if (length > 0)
        commands[length - 1]->set_time_constrain(time_constrain_p);

    //TODO: co se má vracet?
    return true;
};

inline command *square_grid_planner::get_next_command() {
    if (current_cmd_index == command_length_estimate)
        return nullptr;
    else
        return commands[current_cmd_index++];
};

#endif
