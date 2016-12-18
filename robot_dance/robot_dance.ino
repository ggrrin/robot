
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
	//TODO set proper command parser
	cmd_parser = new command_parser_mocap();
}

void loop() {
    robot.start();

	//TODO set proper square dimensions!!!!
	pl = new boe_bot_planner(&robot, 5, 5);
    //TODO: nejspíš přesunout do boebota
    instruction initialInstr = robot.get_instruction_manager().initial_instruction();
    location lastLocation = location(initialInstr);
    robot.set_location(lastLocation);
    command *cmd = nullptr;

//	time_type start_time = millis();
//	while (!cmd_parser->fetch_next())
//	{
//		Serial.print("-> Creating route to { ");
//		Serial.print(cmd_parser->get_current_target().get_x());
//		Serial.print(", ");
//		Serial.print(cmd_parser->get_current_target().get_y());
//		Serial.print(" } firstX ");
//		Serial.println(cmd_parser->is_first_directionX() ? "true" : "false");
//
//		if (!pl->prepare_route(robot.get_location(), location(cmd_parser->get_current_target(), direction::NotSpecified), cmd_parser->is_first_directionX(), 0))
//		{
//			Serial.println("Error invalid arguments in prepare route -> Ignoring....");
//			continue;
//		}
//
//		command* cur_cmd = nullptr;
//		while ((cur_cmd = pl->get_next_command()) != nullptr)
//		{
//			Serial.print("processing: ");
//			Serial.println(cur_cmd->get_name());
//
//			while (!cur_cmd->is_done())
//				cur_cmd->update(millis() - start_time);
//		}
//	}
    /* Execute dance */
    while (1) {
        /* End this loop if push_button was pressed */
        if (robot.get_button().is_pushed()) {
            break;
        }

        if (cmd == nullptr) {
            instruction instr = robot.get_instruction_manager().next_instruction();
            location nextLocation = location(instr);
            pl->prepare_route(lastLocation, nextLocation, instr.horizontalPriority, instr.execution_time);
            lastLocation = nextLocation;
            cmd = pl->get_next_command();

            /* Dance is done, return back home */
            if (cmd == nullptr) {
                break;
            }
        } else if (cmd->is_done()) {
            cmd = pl->get_next_command();
        } else {
            robot.get_sensors().read_sensors();
            cmd->update(millis());
        }
    }

    /* Return to the starting position */
    pl->prepare_route(lastLocation, location(initialInstr), false, 0);

	delete pl;
	delete cmd_parser;
}
