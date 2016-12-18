
#ifndef boe_bot_h_
#define boe_bot_h_

#include <Arduino.h>

#include "location.h"
#include "sensors.hpp"
#include "wheel_control.hpp"
#include "command_parser_eeprom.hpp"
#include "push_button.hpp"


/**
 * Container for the Boe bot robot.
 */
class boe_bot {

    const uint8_t led = 11;

    location location_state;

    wheel_control wheels;
    sensors ir_sensors;
    command_parser cmd_parser;
    push_button button;

public:

    /**
     * Gets the button object.
     *
     * @return The button object.
     */
    const push_button &get_button() const {
        return button;
    }

    /**
     * Gets command parser with access to whole instruction queue.
     *
     * @return The command parser with access to whole instruction queue.
     */
    command_parser *get_command_parser() const {
        return &cmd_parser;
    }

    /**
     * Gets sensors states containing last measurements.
     *
     * @return The sensors states containing last measurements.
     */
    sensors get_sensors() const {
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
    * Zastaví kola robota, kvůli špatné kalibraci k úplnému zastavení dojít nemusí.
    */
    void stop() {
        wheels.left_speed(0);
        wheels.right_speed(0);
    }

    /**
    * Dopředná jízda plnou rychlostí.
    */
    void full_forward() {
        wheels.left_speed(1);
        wheels.right_speed(1);
    }

    /**
    * Move forward with half speed.
    */
    void half_forward() {
        wheels.left_speed(0.5);
        wheels.right_speed(0.5);
    }

    /**
    * Move forward with quarter speed.
    */
    void quarter_forward() {
        wheels.left_speed(0.25);
        wheels.right_speed(0.25);
    }

    /**
    * Otáčení doleva na místě.
    */
    void in_place_left() {
        wheels.left_speed(-1);
        wheels.right_speed(1);
    }

    /**
    * Pomalejší zatáčení doleva obloukem.
    */
    void slightly_left() {
        wheels.left_speed(-0.05);
        wheels.right_speed(0.2);
    }

    /**
    * Nejprudší zatáčení doleva obloukem.
    */
    void sharply_left() {
        wheels.left_speed(0);
        wheels.right_speed(1);
    }

    /**
    * Otáčení doprava na místě.
    */
    void in_place_right() {
        wheels.left_speed(1);
        wheels.right_speed(-1);
    }

    /**
    * Pomalejší zatáčení doprava obloukem.
    */
    void slightly_right() {
        wheels.left_speed(0.2);
        wheels.right_speed(-0.05);
    }

    /**
    * Nejprudší zatáčení doprava obloukem.
    */
    void sharply_right() {
        wheels.left_speed(1);
        wheels.right_speed(0);
    }

    void setup() {
        /* Serial line speed initialization */
        Serial.begin(115200);
        Serial.println("Serial prepared");

        /* Input button & sensors initialization */
        button.init_button();
        ir_sensors.init_sensors();

        /* Output LED and wheel servos initialization */
        pinMode(led, OUTPUT);
        wheels.init_servos();
    }

    void start() {
        /* The length of first button push decides what do do next */
        bool longPress = button.check_long_press_button();

        /* Short button press, load previous dance */
        if (!longPress) {
            if (!cmd_parser.fetch_initial()) {
                Serial.println("Magic does not match!");
                cmd_parser.reset_commands();
            }
            return;
        }

        /* Long button press, read & save new dance until the button is pressed again (second push) */
        do {
            if (Serial.available() > 0) {
                const char value = (const char) Serial.read();
                if (!cmd_parser.store_character(value)) {
                    Serial.println("Incorrect input!");
                    cmd_parser.reset_commands();
                }
            }
        } while (!button.is_pushed());
        set_location(cmd_parser.get_initial_location());

        /* Third button push will start the new dance */
        button.wait_for_button_release();
        button.wait_for_button_push();
    }

};

#endif
