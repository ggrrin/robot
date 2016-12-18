
#ifndef POSITION_HPP
#define POSITION_HPP

#include <Arduino.h>


/**
 * Provides four basic directions, to which robot can be commanded.
 */
enum direction {
    //values and clock wise order is important(used in planner)
    NotSpecified = -1, North, East, South, West
};


/**
 * Stores position on a discrete 2D plane - grid map.
 */
class position {

    /**
     * Horizontal position on a grid map.
     */
    int x;

    /**
     * Vertical position on a grid map.
     */
    int y;

public:

    /**
     * Creates a position on given coordinates.
     *
     * @param x_p Position on horizontal axis.
     * @param y_p Position on vertical axis.
     */
    position(int x_p, int y_p);
    position();

    /**
     * Creates a position as a unit vector from (0;0) based on given direction.
     *
     * @param direction_p Direction specifying unit vector from (0;0).
     */
    explicit position(direction direction_p);

    /**
     * Gets absolute value of position on horizontal axis.
     *
     * @return The absolute value of position on horizontal axis.
     */
    int get_x_abs() const {
        return abs(x);
    }

    /**
     * Gets absolute value of position on vertical axis.
     *
     * @return The absolute value of position on vertical axis.
     */
    int get_y_abs() const {
        return abs(y);
    }

    /**
     * Gets horizontal axis position.
     *
     * @return The horizontal axis position.
     */
    int get_x() const {
        return x;
    }

    /**
     * Gets vertical axis position.
     *
     * @return The vertical axis position.
     */
    int get_y() const {
        return y;
    }

    position operator*(int k) const {
        return position(k * x, k * y);
    }

    position operator+(const position &right) const {
        return position(right.x + x, right.y + y);
    }

    position operator-(const position &right) const {
        return position(x - right.x, y - right.y);
    }

    position &operator+=(const position &right) {
        x += right.x;
        y += right.y;

        return *this;
    }

    position &operator-=(const position &right) {
        x -= right.x;
        y -= right.y;

        return *this;
    }

    /**
     * Computes Manhattan distance to position (0;0).
     *
     * @return The Manhattan distance to position (0;0).
     */
    int get_manhattan_length() const {
        return abs(x) + abs(y);
    }

    /**
     * Gets direction relatively to position (0;0) on vertical axis.
     *
     * @return The direction relatively to position (0;0) on vertical axis.
     */
    direction get_y_direction() const {
        if (y > 0) {
            return direction::North;
        } else if (y < 0) {
            return direction::South;
        }
        return direction::NotSpecified;
    }

    /**
     * Gets direction relatively to position (0;0) on horizontal axis.
     *
     * @return The direction relatively to position (0;0) on horizontal axis.
     */
    direction get_x_direction() const {
        if (x > 0) {
            return direction::East;
        } else if (x < 0) {
            return direction::West;
        }
        return direction::NotSpecified;
    }

};



//class position

position::position(int x_p, int y_p) : x(x_p), y(y_p) {}

inline position::position() : position(0,0) {};

position::position(direction direction_p) :
        x(direction_p == direction::West ? -1 : (direction_p == East ? 1 : 0)),
        y(direction_p == direction::South ? -1 : (direction_p == North ? 1 : 0)) {};

#endif //POSITION_HPP
