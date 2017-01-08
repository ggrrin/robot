

//Enables Serial error messages in other classes (for test purposes macro is not defined)
#define ARUINO

#include <Arduino.h>
#include <Servo.h>

#include "robot_dance.hpp"
#include "location.h"
#include "boe_bot.hpp"
#include "boe_bot_planner.h"
#include "command_parser_mocap.h"


boe_bot robot;
command_parser *cmd_parser = nullptr;
planner *pl = nullptr;


void setup() {
    robot.setup();
}

void go_home_ISR() {
    robot.boe_bot::set_go_home();
    robot.get_button().wait_for_button_release();
    robot.get_button().detach_ISR_on_push();
}

void loop() {
//    cmd_parser = new command_parser_mocap();
    cmd_parser = new command_parser_eeprom();
    robot.start(*cmd_parser);

    location init_location = cmd_parser->get_initial_location();
    robot.set_location(init_location);
    robot.derive_encounters(init_location);

    pl = new boe_bot_planner(&robot);

    time_type start_time = millis();

    robot.get_button().wait_for_button_push();
    robot.get_button().wait_for_button_release();

    /* Interrupt routine for making the robot go home */
    robot.clear_go_home();
    robot.get_button().attach_ISR_on_push(&go_home_ISR);

    /* Execute dance */
    while (cmd_parser->fetch_next() && !robot.do_go_home()) {
        Serial.print(F("\n-> Creating route to { "));
        Serial.print(cmd_parser->get_current_target().get_x());
        Serial.print(F(", "));
        Serial.print(cmd_parser->get_current_target().get_y());
        Serial.print(F(" } firstX="));
        Serial.print(cmd_parser->is_first_directionX() ? F("true") : F("false"));
        Serial.print(F(", time constraint="));
        Serial.println(cmd_parser->get_finish_time_constrain());
        Serial.flush();

        if (!pl->prepare_route(robot.get_location(),
                               location(cmd_parser->get_current_target(), direction::NotSpecified),
                               cmd_parser->is_first_directionX())) {
            Serial.println(F("Error invalid arguments in prepare route -> Ignoring...."));
            continue;
        }

        command *cur_cmd = nullptr;
        while ((cur_cmd = pl->get_next_command()) != nullptr) {
            /* End this loop if push_button was pressed */
            if (robot.do_go_home()) {
                Serial.println(F("button interrupted the execution"));
                Serial.flush();
                break;
            }

            Serial.print(F("processing: "));
            Serial.println(cur_cmd->get_name());

            while (!cur_cmd->is_done()) {
                robot.get_sensors().read_sensors();
                cur_cmd->update();
            }
        }

        /* Waiting / time synchronization for the route - only when not going home */
        if (!robot.do_go_home()) {
            if (cmd_parser->get_finish_time_constrain() * 100 > millis() - start_time) {
                /* Stop before waiting for the next route */
                robot.stop();

                char buff[64];
                sprintf(buff, "waiting until %lu beginning from %lu", cmd_parser->get_finish_time_constrain() * 100,
                        millis() - start_time);
                Serial.println(buff);

                delay(cmd_parser->get_finish_time_constrain() * 100 - millis() + start_time);
            }
            Serial.println(F("route done, fetching next command..."));
        }
    }

    Serial.println(F("Escaped the main execution loop"));
    robot.stop();

    if (robot.do_go_home()) {
        /* Return to the starting position */
        pl->prepare_route(robot.get_location(), init_location, false);
        command *cur_cmd = nullptr;
        while ((cur_cmd = pl->get_next_command()) != nullptr) {
            Serial.print(F("processing: "));
            Serial.println(cur_cmd->get_name());

            while (!cur_cmd->is_done()) {
                robot.get_sensors().read_sensors();
                cur_cmd->update();
            }
        }

        /* Arrived home */
        robot.stop();
    }

    delete cmd_parser;
    delete pl;
}
