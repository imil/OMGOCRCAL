#pragma once

//This file contains geometric primitives and operations on them:
//Point, Box and Vector

typedef int Coord;

//2D point with integer coordinates
struct Point
{
	Coord x;
	Coord y;

	Point() : x(0), y(0) {}
	Point(Coord _x, Coord _y) : x(_x), y(_y) {}

	void scale(double dx, double dy) {
		x = (int) (x * dx);
		y = (int) (y * dy);
	}

	void transfer(int dx, int dy) {
		x += dx;
		y += dy;
	}

	void rotate(double angle_in_radians) {
		double _x = x * cos(angle_in_radians) - y * sin(angle_in_radians);
		double _y = x * sin(angle_in_radians) + y * cos(angle_in_radians);

		x = (int)_x;
		y = (int)_y;
	}

	void rotate(Point center, double angle_in_radians) {
		transfer(-center.x, -center.y);
		rotate(angle_in_radians);
		transfer(center.x, center.y);
	}

	int dist(Point other) const {
		return (int) sqrt((double)((x-other.x) * (x-other.x) + (y-other.y) * (y-other.y)));
	}

	//returns distance between point and ray
	//or -1 if point is in opposite direction
	int dist(Point rayStart, const struct Vector & rayDir) const;
};

inline bool operator == (const Point & first, const Point & second) {
	return first.x == second.x && first.y == second.y;
}

inline bool operator != (const Point & first, const Point & second) {
	return ! (first == second);
}

//Rectangle class
struct Box
{
	Point topLeft;
	Point bottomRight;
	bool empty;

	Coord left() const {
		return topLeft.x; }

	Coord top() const {
		return topLeft.y; }

	Coord bottom() const {
		return bottomRight.y; }

	Coord right() const {
		return bottomRight.x; }

	Coord width() const {
		return bottomRight.x - topLeft.x; }

	Coord height() const {
		return bottomRight.y - topLeft.y; }

	Coord max_size() const {
		return max(width(), height()); }

	Point center() const {
		return Point((topLeft.x + bottomRight.x)/2 , (topLeft.y + bottomRight.y)/2 );
	}

	void scale(double dx, double dy) {
		topLeft.scale(dx,dy);
		bottomRight.scale(dx,dy);
	}

	void transfer(int dx, int dy) {
		topLeft.transfer(dx,dy);
		bottomRight.transfer(dx,dy);
	}

	std::vector<Point> get_spiral(Coord spiral_size) const;
	std::vector<Point> border() const;

	Box() : empty(true) {}
	Box(Point _topLeft, Point _bottomRight) : topLeft(_topLeft), bottomRight(_bottomRight), empty(false) {}
	Box(const RECT & rect) : topLeft(rect.left, rect.top), bottomRight(rect.right, rect.bottom), empty(false) {}

	void add(Point point) {
		if (empty) {
			topLeft = point;
			bottomRight = point;
			empty = false;
		}
		else
		{
			if(point.x < topLeft.x)
				topLeft.x = point.x;
			if(point.x > bottomRight.x)
				bottomRight.x = point.x;
			if(point.y < topLeft.y)
				topLeft.y = point.y;
			if(point.y > bottomRight.y)
				bottomRight.y = point.y;
		}
	}

	void add(const Box & box) {
		add(box.topLeft);
		add(box.bottomRight);
	}

	//Horizontal and vertical tolerance is stored in Globals.h
	bool is_horizontal() const;
	bool is_vertical() const;

	bool contains(Point point) const {
		return point.x >= left() && point.x <= right()
			&& point.y >= top() && point.y <= bottom();
	}

	bool contains(const std::vector<Point> & points) const {
		for(std::vector<Point>::const_iterator it = points.begin(); it != points.end(); ++it)
			if (! contains(*it))
				return false;
		return true;
	}

	operator RECT () const {
		RECT result;
		result.left = left();
		result.right = right();
		result.top = top();
		result.bottom = bottom();
		return result;
	}

	Box shrunk_by(int offset) const {
		Box result(*this);
		result.topLeft.transfer(offset,offset);
		result.bottomRight.transfer(-offset,-offset);
		return result;
	}
};

//Represents direction in 2D space
//uses doubles instead of ints because vectors need to be normalized sometimes
struct Vector
{
	double dx;
	double dy;

	Vector(): dx(0), dy(0) {}
	Vector(Point _start, Point _end) : dx(_end.x-_start.x), dy(_end.y-_start.y) {}
	Vector(double _dx, double _dy) : dx(_dx), dy(_dy) {}
	Vector(const Vector & other) : dx(other.dx), dy(other.dy) {}

	double length() const {
		return sqrt(dx*dx+dy*dy);
	}

	//set length = 1
	void normalize();

	//returns vector turned 90 degrees anticlockwise
	Vector perpendicular() const {
		return Vector(-dy, dx);		
	}

	//Scalar product. Used to find projection length, angle between vectors, etc.
	double dot(const Vector & other) const {
		return dx * other.dx + dy * other.dy;
	}

	//Returns angle in clocks (1-12 clocks correspond to 30 - 360 degrees clockwise)
	double clock_angle() const;
};

//Scalar product as operator
inline double operator * (const struct Vector & first, const struct Vector & second)
{
	return first.dot(second);
}
