
#ifndef WHEEL_CONTROL_HPP
#define WHEEL_CONTROL_HPP

#include <Arduino.h>
#include <Servo.h>

#define MIN_LEFT_SPEED  (1300)
#define MAX_LEFT_SPEED  (1700)
#define MIN_RIGHT_SPEED (1700)
#define MAX_RIGHT_SPEED (1300)
#define SMOOTHING_STEPS (100)
#define SMOOTHING_JUMP  (0.1)


/**
 * Class for basic control of the servos attached to two wheels.
 */
class wheel_control {

private:

    Servo left_wheel;
    Servo right_wheel;

    const uint8_t left_wheel_pin = 12;
    const uint8_t right_wheel_pin = 13;

    double previous_left_speed = 0;
    double previous_right_speed = 0;

    double current_left_speed = 0;
    double current_right_speed = 0;

public:

    /**
     * Initializes the servos for wheel control.
     */
    void init_servos() {
        left_wheel.attach(left_wheel_pin);
        right_wheel.attach(right_wheel_pin);
    }

    /**
     * Stores current speeds as previous ones.
     */
    void store_current_to_previous() {
        previous_left_speed = current_left_speed;
        previous_right_speed = current_right_speed;
    }

    /**
     * Makes the left wheel rotate as desired with smoothing defined by the step number.
     * To achieve desired speed smoothing, this function must be called multiple times,
     * with increasing step number.
     *
     * @param speed Desired speed of the left wheel.
     * @param step_number Used to calculate ratio between the previous and new speed.
     */
    void left_speed(double speed, unsigned long step_number) {
        if (step_number == 0) {
            left_speed(speed);
            return;
        }

        step_number /= 4;

        double new_speed_ratio = min(1, (1. - SMOOTHING_JUMP) / (SMOOTHING_STEPS * SMOOTHING_STEPS)
                                        * step_number * step_number + SMOOTHING_JUMP);
        left_speed((1. - new_speed_ratio) * previous_left_speed + new_speed_ratio * speed);
    }

    /**
     * Makes the left wheel rotate as desired with smoothing defined by the step number.
     * To achieve desired speed smoothing, this function must be called multiple times,
     * with increasing step number.
     *
     * @param speed Desired speed of the right wheel.
     * @param step_number Used to calculate ratio between the previous and new speed.
     */
    void right_speed(double speed, unsigned long step_number) {
        if (step_number == 0) {
            right_speed(speed);
            return;
        }

        step_number /= 4;

        double new_speed_ratio = min(1, (1. - SMOOTHING_JUMP) / (SMOOTHING_STEPS * SMOOTHING_STEPS)
                                        * step_number * step_number + SMOOTHING_JUMP);
        right_speed((1. - new_speed_ratio) * previous_right_speed + new_speed_ratio * speed);
    }

    /**
     * Uses value from [-1; 1] to count and use speed for the left wheel.
     * Value -1 means maximal speed backwards, 1 means maximal speed forwards.
     *
     * @param speed Desired speed of the left wheel from interval [-1; 1], 0 means stop.
     */
    void left_speed(double speed) {
        current_left_speed = speed;
        left_wheel.writeMicroseconds((int) ((MAX_LEFT_SPEED - MIN_LEFT_SPEED) * (speed + 1) / 2.0 + MIN_LEFT_SPEED));
    }

    /**
     * Uses value from [-1; 1] to count and use speed for the right wheel.
     * Value -1 means maximal speed backwards, 1 means maximal speed forwards.
     *
     * @param speed Desired speed of the right wheel from interval [-1; 1], 0 means stop.
     */
    void right_speed(double speed) {
        current_right_speed = speed;
        right_wheel.writeMicroseconds(
                (int) ((MAX_RIGHT_SPEED - MIN_RIGHT_SPEED) * (speed + 1) / 2.0 + MIN_RIGHT_SPEED));
    }

};


#endif //WHEEL_CONTROL_HPP
