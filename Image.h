#pragma once

#include "Point.h"

//Image represents a single continuous set of points
//(well, there may be breaks, but they just come from rotation)
//Image stores information about the box where it is located
//(this box is not necessarily equal to the bounding box)
class Image
{
public:
	Image(void);
	Image(const Box & box) :m_box(box) {}
	Image(const Box & box, const std::vector<Point> & points)
		:m_box(box), m_points(points) {}

	~Image(void);

	void set_box(Box box) {
		m_box = box;
	}

	const Box & box() const {
		return m_box;
	}

	Box minimalBox() const;

	const std::vector<Point> & points() const {
		return m_points; }

	Point firstPoint() const {
		return m_points[0];
	}

	Point lastPoint() const {
		return m_points.back();
	}

	void add(Point point) {
		m_points.push_back(point);
	}

	void addBreak() {
		m_breaks.insert(m_points.size());
	}

	//first-to-last point
	Vector vector() const;

	std::vector<double> getAngleDeltas(int minDist) const;

	void draw(HDC dc, RECT drawRect) const;

	Point project(const Point & pt, const RECT & rect) const;

	void scale(double dx, double dy);
	void transfer(int dx, int dy);

	std::vector<double> collectAngles (Point center) const;

	Point closestPoint(Point rayStart, Vector rayDir) const;

private:
	std::vector<Point> m_points;
	std::set<std::vector<Point>::size_type> m_breaks;
	Box m_box;
};

//This class represents a collection of images in a single box.
//It usually is the character, perhapse in process of drawing
class CompositeImage : public std::vector<Image>
{
public:
	CompositeImage() {}
	CompositeImage(const CompositeImage & other) : std::vector<Image>(other) {}
	CompositeImage(const std::vector<Image> & other) : std::vector<Image>(other) {}

	void add(const Image & image) {
		push_back(image);
	}

	void fit(const Box & box);
	void draw(HDC dc, RECT drawRect) const;
	void scale(double dx, double dy);
	void transfer(int dx, int dy);
	Box minimalBox() const;
};
