#ifndef location_
#define location_

class position
{
	int x;
	int y;

public:
	position(int x_p, int y_p);
};

enum direction
{
	North, South, East, West
};

class location
{
	position position_;
	direction direction_;
public:
	location(int px, int py, direction direction_p);
	location(position position_p, direction direction_p);
};

//definition

position::position(int x_p, int y_p) : x(x_p), y(y_p) {};

location::location(position position_p, direction direction_p) : position_(position_p), direction_(direction_p) {};
location::location(int px, int py, direction direction_p) : position_(position(px,py)), direction_(direction_p) {};

#endif