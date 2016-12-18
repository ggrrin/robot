
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

	time_ttt start_time = millis();
	while (!cmd_parser->fetch_next())
	{
		Serial.print("-> Creating route to { ");
		Serial.print(cmd_parser->get_current_target().get_x());
		Serial.print(", "); 
		Serial.print(cmd_parser->get_current_target().get_y()); 
		Serial.print(" } firstX ");
		Serial.println(cmd_parser->is_first_directionX() ? "true" : "false");

		if (!pl->prepare_route(robot.get_location(), location(cmd_parser->get_current_target(), direction::NotSpecified), cmd_parser->is_first_directionX(), 0))
		{
			Serial.println("Error invalid arguments in prepare route -> Ignoring....");
			continue;
		}

		command* cur_cmd = nullptr;
		while ((cur_cmd = pl->get_next_command()) != nullptr)
		{
			Serial.print("processing: ");
			Serial.println(cur_cmd->get_name());

			while (!cur_cmd->is_done())
				cur_cmd->update(millis() - start_time);
		}
	}

	delete pl;
	delete cmd_parser;
}
