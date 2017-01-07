
#ifndef boe_bot_h_
#define boe_bot_h_

#include <Arduino.h>

#include "location.h"
#include "sensors.hpp"
#include "wheel_control.hpp"
#include "planning.h"
#include "command_parser_eeprom.hpp"
#include "push_button.hpp"

#define BAUD_SPEED  (115200)
#define FULL        (1.0)
#define HALF        (0.5)
#define QUARTER     (0.25)
#define FIFTH       (0.2)
#define VERY_SLOW   (0.05)
#define STOP        (0.0)


/**
 * Container for the Boe bot robot.
 */
class boe_bot {

    const uint8_t led = 11;

    location location_state;

    wheel_control wheels;
    sensors ir_sensors;
    push_button button;

    bool last_move_encountered_left = true;
    bool last_move_encountered_right = true;

    bool smooth_drive = true;
    bool go_home = false;

    void (boe_bot::*last_wheel_control)(void) = nullptr;

    unsigned long num_calls;

public:

    /**
     * Creates a new boe_bot located on (0,0) facing North.
     */
    boe_bot() : location_state(0, 0, direction::North) {}

    /**
     * Finds out if the current position has path to the left.
     *
     * @return If the current position has path to the left.
     */
    bool get_last_move_encountered_left() const {
        return last_move_encountered_left;
    }

    /**
     * Finds out if the current position has path to the right.
     *
     * @return If the current position has path to the right.
     */
    bool get_last_move_encountered_right() const {
        return last_move_encountered_right;
    }

    /**
     * Sets if the last move command encountered left path or not.
     *
     * @param last_move_encountered_left If the last move command encountered left path or not.
     */
    void set_last_move_encountered_left(bool last_move_encountered_left) {
        boe_bot::last_move_encountered_left = last_move_encountered_left;
    }

    /**
     * Sets if the last move command encountered right path or not.
     *
     * @param last_move_encountered_right If the last move command encountered right path or not.
     */
    void set_last_move_encountered_right(bool last_move_encountered_right) {
        boe_bot::last_move_encountered_right = last_move_encountered_right;
    }

    /**
     * Based on the current situation, find out if the first left or right
     * sensor is active, if so, set the path encounter to true.
     * Does not change the encounter state if the sensor is not active.
     */
    void check_for_path_encounters() {
        if (get_sensors().first_left()) {
            last_move_encountered_left = true;
        }
        if (get_sensors().first_right()) {
            last_move_encountered_right = true;
        }
    }

    /**
     * Clears information about the left and right path from
     * the current robot position.
     */
    void clear_last_move_encounters() {
        last_move_encountered_left = false;
        last_move_encountered_right = false;
    }

    /**
     * Gets the button object.
     *
     * @return The button object.
     */
    const push_button &get_button() {
        return button;
    }


    /**
     * Gets sensors states containing last measurements.
     *
     * @return The sensors states containing last measurements.
     */
    sensors &get_sensors() {
        return ir_sensors;
    }

    /**
     * Gets current location of the robot.
     *
     * @return The current location of the robot.
     */
    const location &get_location() const {
        return location_state;
    }

    /**
     * Sets new current location of the robot.
     *
     * @param loc The new current location of the robot.
     */
    void set_location(const location &loc) {
        location_state = loc;
    }

    /**
     * Sets go home to true.
     */
    void set_go_home() {
        go_home = true;
    }

    /**
     * Sets go home to false.
     */
    void clear_go_home() {
        go_home = false;
    }

    /**
     * Gets if the robot should go home.
     *
     * @return If the robot should go home.
     */
    bool do_go_home() const {
        return go_home;
    }

    /**
     * Counts which paths are available based on the current location and direction.
     * Works only if X or Y is 0.
     *
     * @param loc Location to derive from.
     */
    void derive_encounters(const location &loc) {
        /* If one of the locations is 0, we can derive something */
        if (loc.get_position().get_x() == 0) {
            switch (loc.get_direction()) {
                case North:
                    last_move_encountered_left = false;
                    last_move_encountered_right = true;
                    break;
                case East:
                    if (loc.get_position().get_y() == 0) {
                        last_move_encountered_left = true;
                        last_move_encountered_right = false;
                        return;
                    }
                    break;
                case South:
                    last_move_encountered_left = true;
                    last_move_encountered_right = false;
                    break;
                case West:
                    if (loc.get_position().get_y() == 0) {
                        last_move_encountered_left = false;
                        last_move_encountered_right = true;
                        return;
                    }
                    break;
                default:
                    break;
            }
        }
        if (loc.get_position().get_y() == 0) {
            switch (loc.get_direction()) {
                case East:
                    last_move_encountered_left = true;
                    last_move_encountered_right = false;
                    break;
                case West:
                    last_move_encountered_left = false;
                    last_move_encountered_right = true;
                    break;
                default:
                    break;
            }
        }
    }

    /**
     * Turns the LED on.
     */
    void led_on() {
        digitalWrite(led, HIGH);
    }

    /**
     * Turns the LED off.
     */
    void led_off() {
        digitalWrite(led, LOW);
    }

    /**
     * Immediately stops the robot wheels.
     * NOTE: does not have to mean the robot will not move slightly because
     * of wrong wheels calibration.
     */
    void stop() {
        wheels.left_speed(STOP);
        wheels.right_speed(STOP);
        wheels.store_current_to_previous();
        last_wheel_control = &boe_bot::stop;
    }

    /**
     * Stops using the smoothing algorithm.
     * NOTE: Must be called multiple times!
     */
    void stop_smoothly() {
        if (smooth_drive) {
            if (last_wheel_control == &boe_bot::stop_smoothly || last_wheel_control == &boe_bot::stop) {
                ++num_calls;
            } else {
                num_calls = 1;
                last_wheel_control = &boe_bot::stop_smoothly;
                wheels.store_current_to_previous();
            }
        } else {
            num_calls = 0;
        }
        wheels.left_speed(STOP, num_calls);
        wheels.right_speed(STOP, num_calls);
    }

    /**
     * Dopředná jízda plnou rychlostí.
     */
    void full_forward() {
        if (smooth_drive) {
            if (last_wheel_control == &boe_bot::full_forward) {
                ++num_calls;
            } else {
                num_calls = 1;
                last_wheel_control = &boe_bot::full_forward;
                wheels.store_current_to_previous();
            }
        } else {
            num_calls = 0;
        }
        wheels.left_speed(FULL, num_calls);
        wheels.right_speed(FULL, num_calls);
    }

    /**
     * Move forward with half speed.
     */
    void half_forward() {
        if (smooth_drive) {
            if (last_wheel_control == &boe_bot::half_forward) {
                ++num_calls;
            } else {
                num_calls = 1;
                last_wheel_control = &boe_bot::half_forward;
                wheels.store_current_to_previous();
            }
        } else {
            num_calls = 0;
        }
        wheels.left_speed(HALF, num_calls);
        wheels.right_speed(HALF, num_calls);
    }

    /**
     * Move forward with quarter speed.
     */
    void quarter_forward() {
        if (smooth_drive) {
            if (last_wheel_control == &boe_bot::quarter_forward) {
                ++num_calls;
            } else {
                num_calls = 1;
                last_wheel_control = &boe_bot::quarter_forward;
                wheels.store_current_to_previous();
            }
        } else {
            num_calls = 0;
        }
        wheels.left_speed(QUARTER, num_calls);
        wheels.right_speed(QUARTER, num_calls);
    }

    /**
     * Turn around on current spot in anticlockwise direction.
     */
    void in_place_left() {
        if (smooth_drive) {
            if (last_wheel_control == &boe_bot::in_place_left) {
                ++num_calls;
            } else {
                num_calls = 1;
                last_wheel_control = &boe_bot::in_place_left;
                wheels.store_current_to_previous();
            }
        } else {
            num_calls = 0;
        }
        wheels.left_speed(-FULL, num_calls);
        wheels.right_speed(FULL, num_calls);
    }

    /**
     * Turn around on current spot in anticlockwise direction in half speed.
     */
    void in_place_left_half() {
        if (smooth_drive) {
            if (last_wheel_control == &boe_bot::in_place_left_half) {
                ++num_calls;
            } else {
                num_calls = 1;
                last_wheel_control = &boe_bot::in_place_left_half;
                wheels.store_current_to_previous();
            }
        } else {
            num_calls = 0;
        }
        wheels.left_speed(-QUARTER, num_calls);
        wheels.right_speed(QUARTER, num_calls);
    }

    /**
     * Pomalejší zatáčení doleva obloukem.
     */
    void slightly_left() {
        if (smooth_drive) {
            if (last_wheel_control == &boe_bot::slightly_left) {
                ++num_calls;
            } else {
                num_calls = 1;
                last_wheel_control = &boe_bot::slightly_left;
                wheels.store_current_to_previous();
            }
        } else {
            num_calls = 0;
        }
        wheels.left_speed(STOP, num_calls);
        wheels.right_speed(QUARTER, num_calls);
    }

    /**
     * Nejprudší zatáčení doleva obloukem.
     */
    void sharply_left() {
        if (smooth_drive) {
            if (last_wheel_control == &boe_bot::sharply_left) {
                ++num_calls;
            } else {
                num_calls = 1;
                last_wheel_control = &boe_bot::sharply_left;
                wheels.store_current_to_previous();
            }
        } else {
            num_calls = 0;
        }
        wheels.left_speed(STOP, num_calls);
        wheels.right_speed(FULL, num_calls);
    }

    /**
     * Turn around on current spot in clockwise direction.
     */
    void in_place_right() {
        if (smooth_drive) {
            if (last_wheel_control == &boe_bot::in_place_right) {
                ++num_calls;
            } else {
                num_calls = 1;
                last_wheel_control = &boe_bot::in_place_right;
                wheels.store_current_to_previous();
            }
        } else {
            num_calls = 0;
        }
        wheels.left_speed(FULL, num_calls);
        wheels.right_speed(-FULL, num_calls);
    }

    /**
     * Turn around on current spot in clockwise direction with half speed.
     */
    void in_place_right_half() {
        if (smooth_drive) {
            if (last_wheel_control == &boe_bot::in_place_right_half) {
                ++num_calls;
            } else {
                num_calls = 1;
                last_wheel_control = &boe_bot::in_place_right_half;
                wheels.store_current_to_previous();
            }
        } else {
            num_calls = 0;
        }
        wheels.left_speed(QUARTER, num_calls);
        wheels.right_speed(-QUARTER, num_calls);
    }

    /**
    * Pomalejší zatáčení doprava obloukem.
    */
    void slightly_right() {
        if (smooth_drive) {
            if (last_wheel_control == &boe_bot::slightly_right) {
                ++num_calls;
            } else {
                num_calls = 1;
                last_wheel_control = &boe_bot::slightly_right;
                wheels.store_current_to_previous();
            }
        } else {
            num_calls = 0;
        }
        wheels.left_speed(QUARTER, num_calls);
        wheels.right_speed(STOP, num_calls);
//        wheels.left_speed(FIFTH, num_calls);
//        wheels.right_speed(-VERY_SLOW, num_calls);
    }

    /**
    * Nejprudší zatáčení doprava obloukem.
    */
    void sharply_right() {
        if (smooth_drive) {
            if (last_wheel_control == &boe_bot::sharply_right) {
                ++num_calls;
            } else {
                num_calls = 1;
                last_wheel_control = &boe_bot::sharply_right;
                wheels.store_current_to_previous();
            }
        } else {
            num_calls = 0;
        }
        wheels.left_speed(FULL, num_calls);
        wheels.right_speed(STOP, num_calls);
    }

    void setup() {
        /* Serial line speed initialization */
        Serial.begin(BAUD_SPEED);
        Serial.println("\n\nSerial prepared");

        /* Input button & sensors initialization */
        button.init_button();
        ir_sensors.init_sensors();

        /* Output LED and wheel servos initialization */
        pinMode(led, OUTPUT);
        wheels.init_servos();
    }

    void start(command_parser &cmd_parser) {
        /* The length of first button push decides what do do next */
        bool longPress = button.check_long_press_button();

        /* Short button press, load previous dance */
        if (!longPress) {
            Serial.println("Short button press.");
            if (!cmd_parser.fetch_initial()) {
                Serial.println("Magic does not match!");
                cmd_parser.reset_commands();
            }
            return;
        }

        time_type start_time = millis();

        /* Long button press, read & save new dance until the button is pressed again (second push) */
        Serial.println("Waiting for data...");
        do {
            if (Serial.available() > 0) {
                const char value = (const char) Serial.read();
                if (!cmd_parser.store_character(value)) {
                    Serial.println("Incorrect input!");
                    cmd_parser.reset_commands();
                    break;
                }
            }

            /* blinking LED */
            time_type diff_time = millis() - start_time;
            if (diff_time > 1000) {
                start_time = millis();
            } else if (diff_time > 500) {
                led_on();
            } else if (diff_time <= 500) {
                led_off();
            }
        } while (!button.is_pushed());
        led_off();
        Serial.println("No more input data is expected.");

        /* Next button push will start the new dance */
        button.wait_for_button_release();

        /* Revert parser to start of the sequence and fetch the new initial location */
        cmd_parser.fetch_initial();
    }

};

#endif
