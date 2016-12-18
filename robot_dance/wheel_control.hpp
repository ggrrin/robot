
#ifndef WHEEL_CONTROL_HPP
#define WHEEL_CONTROL_HPP

#include <Arduino.h>
#include <Servo.h>

#define MIN_LEFT_SPEED  (1300)
#define MAX_LEFT_SPEED  (1700)
#define MIN_RIGHT_SPEED (1700)
#define MAX_RIGHT_SPEED (1300)


/**
 * Class for basic control of the servos attached to two wheels.
 */
class wheel_control {

private:

    Servo leftWheel;
    Servo rightWheel;

    const uint8_t leftWheelPin = 12;
    const uint8_t rightWheelPin = 13;

public:

    /**
     * Initializes the servos for wheel control.
     */
    void init_servos() {
        leftWheel.attach(leftWheelPin);
        rightWheel.attach(rightWheelPin);
    }

    /**
     * Uses value from [-1; 1] to count and use speed for the left wheel.
     * Value -1 means maximal speed backwards, 1 means maximal speed forwards.
     *
     * @param speed Desired speed of the left wheel from interval [-1; 1], 0 means stop.
     */
    void left_speed(double speed) {
        leftWheel.writeMicroseconds((int) ((MAX_LEFT_SPEED - MIN_LEFT_SPEED) * (speed + 1) / 2.0 + MIN_LEFT_SPEED));
    }

    /**
     * Uses value from [-1; 1] to count and use speed for the right wheel.
     * Value -1 means maximal speed backwards, 1 means maximal speed forwards.
     *
     * @param speed Desired speed of the right wheel from interval [-1; 1], 0 means stop.
     */
    void right_speed(double speed) {
        rightWheel.writeMicroseconds((int) ((MAX_RIGHT_SPEED - MIN_RIGHT_SPEED) * (speed + 1) / 2.0 + MIN_RIGHT_SPEED));
    }

};


#endif //WHEEL_CONTROL_HPP
