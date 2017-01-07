
#ifndef SENSORS_HPP
#define SENSORS_HPP

#include <Arduino.h>

#define BLACK   (0)
#define WHITE   (1)


/**
 * Class for using attached infra-red sensors.
 */
class sensors {

private:

    /**
     * Number of attached IR sensors.
     */
    static const uint8_t NUM_SENSORS = 5;

    /**
     * Pins attached to sensors.
     */
    const uint8_t sensors[NUM_SENSORS] = {3, 4, 5, 6, 7};

    /**
     * Last measured sensor values.
     */
    int sensor_values[NUM_SENSORS] = {0, 0, 0, 0, 0};

public:

    /**
     * Initializes pins with attached sensors.
     */
    void init_sensors() {
        for (int i = 0; i < NUM_SENSORS; i++) {
            pinMode(sensors[i], INPUT);
        }
    }

    /**
     * Reads and stores values of each sensor.
     */
    void read_sensors() {
        for (int i = 0; i < NUM_SENSORS; i++) {
            sensor_values[i] = digitalRead(sensors[i]);
        }
    }

    /**
     * Checks if the most left sensor is reading black color.
     *
     * @return If the most left sensor is reading black color.
     */
    boolean first_left() const {
        return sensor_values[0] == BLACK;
    }

    /**
     * Checks if the second left sensor is reading black color.
     *
     * @return If the second left sensor is reading black color.
     */
    boolean second_left() const {
        return sensor_values[1] == BLACK;
    }

    /**
     * Checks if the middle sensor is reading black color.
     *
     * @return If the middle sensor is reading black color.
     */
    boolean middle() const {
        return sensor_values[2] == BLACK;
    }

    /**
     * Checks if the second right sensor is reading black color.
     *
     * @return If the second right sensor is reading black color.
     */
    boolean second_right() const {
        return sensor_values[3] == BLACK;
    }

    /**
     * Checks if the first right sensor is reading black color.
     *
     * @return If the first right sensor is reading black color.
     */
    boolean first_right() const {
        return sensor_values[4] == BLACK;
    }

    /**
     * Checks if the first and second left sensors are reading black color.
     *
     * @return If the first and second left sensors are reading black color.
     */
    boolean left_part() const {
        return first_left() && second_left();
    }

    /**
     * Checks if the first and second right sensors are reading black color.
     *
     * @return If the first and second right sensors are reading black color.
     */
    boolean right_part() const {
        return first_right() && second_right();
    }

    /**
     * Checks if the middle and both seconds sensors are reading black color.
     *
     * @return If the middle and both seconds sensors are reading black color.
     */
    boolean middle_part() const {
        return second_left() && middle() && second_right();
    }

    /**
     * Checks if the all sensors are reading black color.
     *
     * @return If the all sensors are reading black color.
     */
    boolean each() const {
        return left_part() && middle() && right_part();
    }

};

#endif //SENSORS_HPP
