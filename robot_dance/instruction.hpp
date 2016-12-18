
#ifndef INSTRUCTION_HPP
#define INSTRUCTION_HPP

#include <Arduino.h>

#include "position.hpp"
#include "robot_dance.hpp"


/**
 * Instruction containing information about desired destination and
 * absolute time, in which this instruction should be left.
 */
class instruction {
public:

    /**
     * Desired final position after execution of this instruction.
     */
    position desiredPosition;

    /**
     * Initial direction is available only for the first instruction.
     */
    direction initialDirection;

    /**
     * Defines if the move on horizontal axis should be executed first.
     */
    bool horizontalPriority;

    /**
     * Absolute time for execution of this instruction.
     */
    time_type execution_time;

    /**
     * Creates an initial instruction.
     *
     * @param desiredPosition Initial position.
     * @param initialDirection Initial direction.
     */
    instruction(const position &desiredPosition, direction initialDirection) :
            desiredPosition(desiredPosition), initialDirection(initialDirection), horizontalPriority(false),
            execution_time(0) {}

    /**
     * Creates an ordinary instruction.
     *
     * @param desiredPosition Desired final position.
     * @param horizontalPriority Priority of the horizontal axis.
     * @param execution_time Maximum absolute time for execution.
     */
    instruction(const position &desiredPosition, bool horizontalPriority, time_type execution_time) :
            desiredPosition(desiredPosition), initialDirection(nullptr), horizontalPriority(horizontalPriority),
            execution_time(execution_time) {}

    String to_string() {
        if (initialDirection != nullptr) {
            
        } else {

        }
    }
};

#endif //INSTRUCTION_HPP
