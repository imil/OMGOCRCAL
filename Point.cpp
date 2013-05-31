#include "stdafx.h"

#include "Globals.h"
#include "Point.h"

std::vector<Point> Box::border() const
{
	std::vector<Point> result;

	for(Coord x = topLeft.x; x < bottomRight.x; ++x)
		result.push_back(Point(x,topLeft.y));

	for(Coord y = topLeft.y; y < bottomRight.y; ++y)
		result.push_back(Point(bottomRight.x,y));

	for(Coord x = bottomRight.x; x > topLeft.x; --x)
		result.push_back(Point(x,bottomRight.y));

	for(Coord y = bottomRight.y; y > topLeft.y; --y)
		result.push_back(Point(topLeft.x,y));

	return result;
}

std::vector<Point> Box::get_spiral(Coord spiral_size) const
{
	std::vector<Point> result;

	
	Coord left = max(center().x - spiral_size, topLeft.x);
	Coord right = min(center().x + spiral_size, bottomRight.x);
	Coord top = max(center().y - spiral_size, topLeft.y);
	Coord bottom = min(center().y + spiral_size, bottomRight.y);

	return Box(Point(top,left), Point(bottom,right)).border();
}

double Vector::clock_angle() const
{
	if (abs(dx) < Globals::eps() && abs(dy) < Globals::eps())
		return 0;

	Vector norm = *this;

	norm.normalize();

	double cosA = norm.dot(Vector(0,-1));

	double alpha = acos(min(1,max(-1,cosA)));

	if (norm.dx < 0)
		alpha = 2 * M_PI - alpha;

	double degrees = alpha * 180.0 / M_PI;

	double clk = Globals::interpolate(alpha, 0, 2 * M_PI, 0, 12);

	return clk;
}

int Point::dist(Point rayStart, const struct Vector & rayDir) const
{
	Vector vecToPoint(rayStart, *this);

	if (vecToPoint * rayDir < 0)
		return -1;

	Vector perp = rayDir.perpendicular();
	perp.normalize();

	double dist = abs(vecToPoint * perp);

	return (int) dist;
}

bool Box::is_horizontal() const {
	return width() > Globals::horizontalOrVerticalMinimalRatio() * height();
}

bool Box::is_vertical() const {
	return height() > Globals::horizontalOrVerticalMinimalRatio() * width();
}

void Vector::normalize() {
	double len = length();

	if (len > Globals::eps())
	{
		dx /= len;
		dy /= len;
	}
}


