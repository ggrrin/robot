#ifndef location_
#define location_

enum direction
{
	//values and clock wise order is important(used in planner)
	NotSpecified = -1, North, East, South, West
};

class position
{
	int x;
	int y;

public:
	position(int x_p, int y_p);
	explicit position(direction direction_p);


	int get_x_abs() const
	{
		return abs(x);
	}

	int get_y_abs() const
	{
		return abs(y);
	}

	int get_x() const
	{
		return x;
	}

	int get_y() const
	{
		return y;
	}

	position operator*(int k) const
	{
		return position(k*x, k*y);
	}

	position operator+(const position& right) const
	{
		return position(right.x + x, right.y + y);
	}

	position operator-(const position& right) const
	{
		return position(x - right.x, y - right.y);
	}


	position& operator+=(const position& right)
	{
		x += right.x;
		y += right.y;
		return *this;
	}

	position& operator-=(const position& right)
	{
		x -= right.x;
		y -= right.y;
		return *this;
	}

	int get_manhattan_length() const
	{
		return abs(x) + abs(y);
	}

	direction get_y_direction() const
	{
		if (y > 0)
			return direction::East;
		else if (y < 0)
			return direction::West;
		else
			direction::NotSpecified;
	}

	direction get_x_direction() const
	{
		if (x > 0)
			return direction::East;
		else if (x < 0)
			return direction::West;
		else
			direction::NotSpecified;
	}
};

class location
{
	position position_;
	direction direction_;
public:
	location();
	location(int px, int py, direction direction_p);
	location(position position_p, direction direction_p);

	position get_position() const
	{
		return position_;
	}

	direction get_direction() const
	{
		return direction_;
	};

};

//definition

position::position(int x_p, int y_p) : x(x_p), y(y_p) {};
position::position(direction direction_p) :
	x(direction_p == direction::West ? -1 : (direction_p == East ? 1 : 0)),
	y(direction_p == direction::South ? -1 : (direction_p == North ? 1 : 0)) {};

location::location() : location(0, 0, direction::NotSpecified) {};
location::location(position position_p, direction direction_p) : position_(position_p), direction_(direction_p) {};
location::location(int px, int py, direction direction_p) : position_(position(px, py)), direction_(direction_p) {};

#endif