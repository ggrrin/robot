
#include "robot_dance.hpp"

//Enables Serial error messages in other classes (for test purposes macro is not defined)
#define ARUINO

#include "location.h"
#include "boe_bot_planner.h"
#include "command_parser_mocap.h"

boe_bot robot;
command_parser* cmd_parser = nullptr;
planner* pl = nullptr;


void setup() {
	robot.setup();
}

void loop() {
    robot.start();

//    cmd_parser = new command_parser_mocap();
    cmd_parser = robot.get_command_parser();

	//TODO set proper square dimensions!!!!
    //NOTE: dimensions do not have to be known at the beginning
	pl = new boe_bot_planner(&robot, 5, 5);

    time_type start_time = millis();
    bool button_pushed = false;

    /* Execute dance */
    while (!cmd_parser->fetch_next() && !button_pushed) {
        Serial.print("-> Creating route to { ");
        Serial.print(cmd_parser->get_current_target().get_x());
        Serial.print(", ");
        Serial.print(cmd_parser->get_current_target().get_y());
        Serial.print(" } firstX ");
        Serial.println(cmd_parser->is_first_directionX() ? "true" : "false");

        if (!pl->prepare_route(robot.get_location(),
                               location(cmd_parser->get_current_target(), direction::NotSpecified),
                               cmd_parser->is_first_directionX(), cmd_parser->get_finish_time_constrain())) {
            Serial.println("Error invalid arguments in prepare route -> Ignoring....");
            continue;
        }

        command *cur_cmd = nullptr;
        while ((cur_cmd = pl->get_next_command()) != nullptr) {
            /* End this loop if push_button was pressed */
            if (robot.get_button().is_pushed()) {
                Serial.print("button interrupted the execution");
                button_pushed = true;
                break;
            }

            Serial.print("processing: ");
            Serial.println(cur_cmd->get_name());

            while (!cur_cmd->is_done())
                cur_cmd->update(millis() - start_time);
        }
    }

    /* Return to the starting position */
    pl->prepare_route(robot.get_location(), cmd_parser->get_initial_location(), false, 0);

	delete pl;
	delete cmd_parser;
}
