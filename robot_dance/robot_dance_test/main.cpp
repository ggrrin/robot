#include "iostream"
#include "planner_test.h"
#include <memory>
#include <vector>
#include <tuple>

using namespace std;



void test_planner_sequence(location initial_location, int widht, int height, const vector<tuple<position, bool>>& coors)
{
	auto* ctx = new context(initial_location);
	auto pl = make_unique<test_planner>(ctx);

	cout << "->=>-> Testing square(" << widht << ", " << height << ") ||  " << *ctx;

	for (const auto& coor : coors)
	{
		cout << "-> Creating route to { " << get<0>(coor).get_x() << ", " << get<0>(coor).get_y() << " } firstX " << get<1>(coor) << endl;

		if (!pl->prepare_route(ctx->get_location(), location(get<0>(coor), direction::NotSpecified), get<1>(coor)))
		{
			cout << "Error invalid arguments in prepare route -> Ignoring...." << endl;
			continue;
		}

		command* cur_cmd = nullptr;
		while ((cur_cmd = pl->get_next_command()) != nullptr)
		{
			cout << "processing: " << cur_cmd->get_name() << endl;

			while (!cur_cmd->is_done())
				cur_cmd->update();

			std::cout << *ctx;
		}

	}

	delete ctx;
}


void test_planner_2moves()
{
	vector<tuple<position, bool>> coors;

	test_planner_sequence(location(0, 0, direction::South), 5, 6, coors = {
		make_tuple(position(2,3), false),
		make_tuple(position(4,1), true)
	});
}

void test_planner_2moves_invert()
{
	vector<tuple<position, bool>> coors;

	test_planner_sequence(location(0, 0, direction::West), 5, 6, coors = {
		make_tuple(position(2,3), true),
		make_tuple(position(4,1), true)
	});
}



void test_planner_go_first_0()
{
	vector<tuple<position, bool>> coors;

	test_planner_sequence(location(2, 3, direction::South), 5, 6, coors = {
		make_tuple(position(4,3), false),
	});
}

void test_planner_dead_moves()
{
	vector<tuple<position, bool>> coors;

	test_planner_sequence(location(0, 0, direction::North), 5, 6, coors = {
		make_tuple(position(2,3), false),
		make_tuple(position(2,3), true),
		make_tuple(position(2,3), false),
		make_tuple(position(4,1), true)
	});
}

void test_planner_outside_grid()
{
	vector<tuple<position, bool>> coors;

	test_planner_sequence(location(0, 0, direction::North), 5, 6, coors = {
		make_tuple(position(2,3), false),
		make_tuple(position(1,6), false),
		make_tuple(position(3,-1), true),
		make_tuple(position(5,5), true),
		make_tuple(position(-1,2), false),
		make_tuple(position(3,3), true),
	});
}

void test_planner_border_turns()
{
	vector<tuple<position, bool>> coors;

	test_planner_sequence(location(0, 0, direction::West), 3, 3, coors = {
		make_tuple(position(2,0), false),
		make_tuple(position(2,2), false),
		make_tuple(position(0,2), true),
		make_tuple(position(0,0), true),
	});
}

void test_planner_border_forward_back()
{
	vector<tuple<position, bool>> coors;

	test_planner_sequence(location(0, 0, direction::West), 3, 3, coors = {
		make_tuple(position(2,0), false),
		make_tuple(position(0,0), false),
		make_tuple(position(2,0), true),
	});
}


int main(int argc, char* argv[])
{
	test_planner_2moves();
	test_planner_2moves_invert();
	test_planner_go_first_0();
	test_planner_dead_moves();
	//test_planner_outside_grid();
	test_planner_border_turns();
	test_planner_border_forward_back();

	cout << "Press anything to exit..." << endl;
	cin.get();
}