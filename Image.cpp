#include "stdafx.h"

#include "Globals.h"
#include "Image.h"

Image::Image(void)
{
}

Image::~Image(void)
{
}

Box Image::minimalBox() const
{
	Box result;
	for(std::vector<Point>::const_iterator it = m_points.begin(); it != m_points.end(); ++it)
		result.add(*it);
	return result;
}

Vector Image::vector() const
{
	if (m_points.size() < 2)
		return Vector(0,0);
	else
		return Vector(m_points.front(), m_points.back());
}

std::vector<double> Image::getAngleDeltas(int minDist) const
{
	std::vector<double> result;

	Vector previousVec(0,0);

	std::vector<Point>::const_iterator it_start = m_points.begin();
	while(it_start != m_points.end())
	{
		Point start = *it_start;
		std::vector<Point>::const_iterator it_end = it_start+1;
		while(it_end != m_points.end() && start.dist(*it_end) <= minDist)
		{
			++it_end;
		}
		Point end = *(it_end-1);

		Vector currentVec(start,end);
		currentVec.normalize();

		if (previousVec.length() != 0)
		{
			double cosA = max(min(currentVec * previousVec,1),-1);
			double da = acos(cosA);
			result.push_back(da);
		}

		it_start = it_end;
		previousVec = currentVec;
	}

	return result;
}

Point Image::project(const Point & pt, const RECT & rect) const
{
	return Point(
		(Coord)Globals::interpolate(pt.x, m_box.topLeft.x, m_box.bottomRight.x, rect.left, rect.right),
		(Coord)Globals::interpolate(pt.y, m_box.topLeft.y, m_box.bottomRight.y, rect.top, rect.bottom));

}

void Image::draw(HDC dc, RECT drawRect) const
{
	if (! m_points.empty() )
	{
		POINT oldPoint;
		Point start = project(* m_points.begin(), drawRect);
		MoveToEx(dc, start.x, start.y, &oldPoint);

		for(unsigned int i = 1; i < m_points.size(); ++i)
		{
			const Point & unprojectedPoint = m_points[i];
			Point nextPoint = project(unprojectedPoint, drawRect);
			if ( m_breaks.find(i) == m_breaks.end())
			{
				LineTo(dc, nextPoint.x, nextPoint.y);  
			}
			else
			{
				MoveToEx(dc, nextPoint.x, nextPoint.y, NULL);
			}
		}

		MoveToEx(dc, oldPoint.x, oldPoint.y, NULL);
	}
}

void Image::scale(double dx, double dy)
{
	for(std::vector<Point>::iterator it = m_points.begin(); it != m_points.end(); ++it)
	{
		Point & point = *it;
		point.scale(dx,dy);
	}
}

void Image::transfer(int dx, int dy)
{
	for(std::vector<Point>::iterator it = m_points.begin(); it != m_points.end(); ++it)
	{
		Point & point = *it;
		point.transfer(dx,dy);
	}
}

void CompositeImage::draw(HDC dc, RECT drawRect) const
{
	for(const_iterator it = begin(); it != end(); ++it)
		it->draw(dc, drawRect);
}

void CompositeImage::fit(const Box & box)
{
	Box realBox = minimalBox();

	double dx = (double)box.width() / realBox.width();
	double dy = (double)box.height() / realBox.height();

	double dmin = min(dx,dy);

	if (dmin < 1.0)
	{
		scale(dmin, dmin);
		realBox = minimalBox();
	}

	if (realBox.height() > 0.3 * box.height()) //the only shape that needn't be centered is 
											   //decimal point, and here we check for it
	{
		Point currentCenter = realBox.center();
		Point requiredCenter = box.center();

		transfer(requiredCenter.x - currentCenter.x, requiredCenter.y - currentCenter.y);
	}
}

void CompositeImage::scale(double dx, double dy)
{
	for(iterator it = begin(); it != end(); ++it)
		it->scale(dx,dy);
}

void CompositeImage::transfer(int dx, int dy)
{
	for(iterator it = begin(); it != end(); ++it)
		it->transfer(dx,dy);
}

Box CompositeImage::minimalBox() const
{
	Box result;

	for(const_iterator it = begin(); it != end(); ++it)
		result.add(it->minimalBox());

	return result;
}


struct angle_collector {
	angle_collector(Point _center, std::vector<double> & _result) : center(_center), result(_result) {}

	void operator () (const Point & point) {
		result.push_back( Vector(center, point).clock_angle() );
	}

	Point center;
	std::vector<double> & result;
};

std::vector<double> Image::collectAngles (Point center) const
{
	std::vector<double> result;

	angle_collector collect_angles(center, result);
	std::for_each(m_points.begin(), m_points.end(), collect_angles);

	std::sort(result.begin(), result.end());

	return result;
}

Point Image::closestPoint(Point rayStart, Vector rayDir) const
{
	Point result;
	double dist;
	bool found = false;

	for (std::vector<Point>::const_iterator it = m_points.begin(); it != m_points.end(); ++it)
	{
		Point point = *it;

		double curr_dist = point.dist(rayStart, rayDir);

		if (curr_dist >= 0)
		{
			if ( !found || curr_dist < dist )
			{
				found = true;
				dist = curr_dist;
				result = point;
			}
		}
	}

	return result;
}