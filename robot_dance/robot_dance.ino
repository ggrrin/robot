#include "location.h"
#include "boe_bot_planner.h"

boe_bot robot;

void setup() {
	robot.setup();
}

void loop() {
	robot.start();

	planner* pl = new boe_bot_planner(&robot, 5, 5);
	command* cmd = nullptr;

	while (1) {
		if (cmd == nullptr)
		{
			if (!pl->prepare_route(location(0, 0, direction::North), location(4, 4, direction::West), true, 0))
			{
				//TODO what to do
				robot.led_on();
				break;
			}
		}
		else if (cmd->is_done())
		{
			cmd = pl->get_next_command();
		}
		else
		{
			robot.read_sensors();
			cmd->update(millis());
		}
	}

	delete pl;
}
