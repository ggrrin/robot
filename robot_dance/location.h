#ifndef location_h_
#define location_h_

//#include <Arduino.h>

#include "position.hpp"


/**
 * Provides information both about position as well as direction of the robot.
 */
class location {

    /**
     * Current position of the robot.
     */
    position position_;

    /**
     * Current direction of the robot.
     */
    direction direction_;

public:

    /**
     * Creates a location at (0;0) with unspecified direction.
     */
    location();

    /**
     * Creates a location at given position towards given direction.
     *
     * @param px Position on horizontal axis.
     * @param py Position on vertical axis.
     * @param direction_p Direction on this location.
     */
    location(int px, int py, direction direction_p);

    /**
     * Creates a location from existing position towards given direction.
     *
     * @param position_p Position of this location.
     * @param direction_p Direction on this location.
     */
    location(position position_p, direction direction_p);

    /**
     * Sets the location at given position towards given direction.
     *
     * @param px Position on horizontal axis.
     * @param py Position on vertical axis.
     * @param direction_p Direction on this location.
     */
    void set_location(int px, int py, direction direction_p) {
        position_ = position(px, py);
        direction_ = direction_p;
    }

    /**
     * Gets current position.
     *
     * @return The current position.
     */
    position get_position() const {
        return position_;
    }

    /**
     * Gets current direction.
     *
     * @return The current direction.
     */
    direction get_direction() const {
        return direction_;
    };

};

bool operator==(const location& lhs, const location& rhs)
{
	return lhs.get_direction() == rhs.get_direction() && lhs.get_position() == rhs.get_position();
}

bool operator!=(const location& lhs, const location& rhs)
{
	return !(lhs == rhs);
}


//class location

location::location() : location(0, 0, direction::NotSpecified) {};

location::location(position position_p, direction direction_p) : position_(position_p), direction_(direction_p) {};

location::location(int px, int py, direction direction_p) : position_(position(px, py)), direction_(direction_p) {};

#endif