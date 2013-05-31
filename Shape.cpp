#include "stdafx.h"

#include "Shape.h"

#include "Globals.h"

Shape::Shape(void)
	:type(shNone), size(ssAny), direction(ddHorizontal), start(0), end(0)
{
}

Shape::~Shape(void)
{
}

Probability probability_of_point_being_on_line(const Point & point, const Point & lineStart, const Point & lineEnd)
{
	Vector lineVec(lineStart, lineEnd);
	Vector perp = lineVec.perpendicular();
	perp.normalize();

	Vector vecToPoint(lineStart, point);

	double dist = abs(vecToPoint * perp);

	return (dist < Globals::lineThreshold() * lineVec.length()) ? 100 : 0;
}

struct line_match_prob : public std::unary_function<const Point &, void> {
	line_match_prob(Point _first, Point _last, std::vector<Probability> & _result) : first(_first), last(_last), result(_result) {
	}
	void operator () (const Point & point) {
		Probability prob = probability_of_point_being_on_line(point, first, last);
		result.push_back(prob);
	}

	Point first;
	Point last;
	std::vector<Probability> & result;
};

void addResults(const std::vector<std::pair<Shape::ShapeWithAnchors, Probability> > & from, std::vector<std::pair<Shape::ShapeWithAnchors, Probability> > & to)
{
	std::copy(from.begin(), from.end(), std::back_inserter(to));
}

std::vector<std::pair<Shape::ShapeWithAnchors, Probability> > Shape::analyze(const Image & img)
{
	std::vector<std::pair<ShapeWithAnchors, Probability> > result;

	addResults(analyzeLine(img), result);

	addResults(analyzeCircular(img), result);

	return result;
}

Shape::ShapeWithAnchors Shape::mostLikely(const Image & img)
{
	std::vector<std::pair<ShapeWithAnchors, Probability> > allVariants = analyze(img);

	if (allVariants.empty())
		return ShapeWithAnchors();
	else
		return allVariants.front().first;
}

struct probability_less : public std::binary_function <
	std::pair<Shape::ShapeWithAnchors, Probability>, std::pair<Shape::ShapeWithAnchors, Probability>,
	bool >
{
	result_type operator () ( const first_argument_type & first, const second_argument_type & second )
	{
		return second.second < first.second;
	}
};

std::vector<std::pair<Shape::ShapeWithAnchors, Probability> > Shape::analyzeLine(const Image & img)
{
	std::vector<std::pair<ShapeWithAnchors, Probability> > result;

	Box realBox = img.minimalBox();
	
	if (!realBox.empty)
	{
		std::vector<Probability> individualResults;

		std::for_each(img.points().begin(), img.points().end(), line_match_prob(img.firstPoint(), img.lastPoint(), individualResults));

		int mismatches = 0;
		for(std::vector<Probability>::iterator it = individualResults.begin(); it != individualResults.end(); ++it)
			if ((*it).percent() < 80)
				++mismatches;

		double mismatchRatio = (double) mismatches / individualResults.size();

		Probability prob(mismatchRatio < 0.1 ? 100 : 0);

		if ( Globals::probabilityToConsiderShape() < prob)
		{
			Shape shape = Shape::createLine();
			shape.direction = analyzeLineDirection(img);
			result.push_back(std::make_pair(ShapeWithAnchors(shape,anchorsForLine(shape, img)), prob));
		}
	}

	std::sort(result.begin(), result.end(), probability_less() );

	return result;
}

Anchors Shape::anchorsForLine(const Shape & shape, const Image & img)
{
	Anchors result;

	if (shape.direction != ddVertical)
	{
		result[ptLeft] = leftPoint(img);
		result[ptRight] = rightPoint(img);
	}
	if (shape.direction != ddHorizontal)
	{
		result[ptTop] = topPoint(img);
		result[ptBottom] = bottomPoint(img);
	}
	result[ptMiddle] = middlePoint(img);

	return result;
}

bool Shape::containsClock(int clk) const
{
	if (type == shCircle)
	{
		return true;
	}
	else if (type == shSegment)
	{
		int t_start = start;
		int t_end = end;
		while(t_end < t_start)
			t_end += 12;

		for(int i = t_start; i <= t_end; ++i)
			if (i % 12 == clk % 12)
				return true;

		return false;
	}
	else
	{
		return false;
	}
}

Point Shape::topPoint(const Image & img)
{
	return img.firstPoint().y < img.lastPoint().y ?
		img.firstPoint() : img.lastPoint();
}

Point Shape::rightPoint(const Image & img)
{
	return img.firstPoint().x > img.lastPoint().x ?
		img.firstPoint() : img.lastPoint();
}

Point Shape::leftPoint(const Image & img)
{
	return img.firstPoint().x < img.lastPoint().x ?
		img.firstPoint() : img.lastPoint();
}

Point Shape::bottomPoint(const Image & img)
{
	return img.firstPoint().y > img.lastPoint().y ?
		img.firstPoint() : img.lastPoint();
}

Point Shape::middlePoint(const Image & img) //TODO: return point on line
{
	return Box(img.firstPoint(), img.lastPoint()).center();
}

Direction Shape::analyzeLineDirection(const Image & img)
{
	Box realBox = img.minimalBox();

	if (realBox.is_horizontal())
		return ddHorizontal;

	if (realBox.is_vertical())
		return ddVertical;

	Vector lineVec = img.vector();

	if (abs(lineVec * Vector(1,1)) > abs(lineVec * Vector(1,-1)))
		return ddBackslash;
	else
		return ddSlash;
}

template <typename T, typename R, typename C> R iterate_in_spyral_from_center(Box box, T test_fun, C compare)
{
	R result = test_fun(box.center());
	Point center;

	for (int spiral_size = 0; spiral_size < box.max_size() / 2; ++spiral_size)
	{
		R iterationProb = result;
		Point iterationCenter;

		std::vector<Point> spiral = box.get_spiral(spiral_size);

		for(std::vector<Point>::iterator it = spiral.begin(); it != spiral.end(); ++it)
		{
			Point point = *it;
			R pointProb = test_fun(point);
			if (compare(iterationProb, pointProb))
			{
				iterationProb = pointProb;
				iterationCenter = point;
			}
		}

		if (compare(iterationProb, result))
		{
			break; //this means that we are getting farther from center
		}
		else
		{
			result = iterationProb;
			center = iterationCenter;
		}
	}

	return result;
}

struct distance_collector : public std::unary_function<Point, void>
{
	distance_collector(Point center) : m_center(center) {}

	void operator () (Point point) {
		m_distances.push_back(point.dist(m_center));
	}

	Point m_center;
	std::vector<int> m_distances;
};

struct ring_width_detector : public std::unary_function<int, void>
{
	ring_width_detector() : empty(true) {}

	void operator () (int distance)
	{
		if (empty)
		{
			min_dist = max_dist = distance;
			empty = false;
		}
		else
		{
			if (min_dist > distance)
				min_dist = distance;
			if (max_dist < distance)
				max_dist = distance;
		}
	}
	int width() {
		return max_dist - min_dist;
	}

	bool empty;
	int  min_dist;
	int  max_dist;
};

double Globals::interpolate(double val, double minSrc, double maxSrc, double minDst, double maxDst)
{
	return minDst + (val - minSrc) * (maxDst - minDst) / (maxSrc - minSrc);
}

Probability circle_probability_from_ring_width(const Image & img, int width)
{
	if (width == 0)
		return 100;

	if (width > Globals::circleThreshhold() * img.minimalBox().max_size() )
		return 0;

	return (int)Globals::interpolate(width, 0, (int)(Globals::circleThreshhold() * img.minimalBox().max_size()), 100, 60);
}

struct circle_detector : public std::unary_function<const Point &, Probability>
{
	circle_detector(const Image & img) : m_image(img) {}

	int operator () (const Point & center)
	{
		distance_collector dc(center);
		dc = std::for_each(m_image.points().begin(), m_image.points().end(), dc);

		ring_width_detector ring_width = std::for_each(dc.m_distances.begin(), dc.m_distances.end(), ring_width_detector());

		int width = ring_width.width();
		return width;
		//return circle_probability_from_ring_width(m_image, ring_width.width());
	}

	const Image & m_image;
};

struct more_int
{
	bool operator () (int first, int second) {
		return first > second;
	}
};

int get_clock_for_angle(double angle)
{
	int result = (int)angle;
	if (result == 0 || result == 13)
		result = 12;
	return result;
}

bool is_enough_points_in_clock(int q)
{
	return q > 1;
}

std::vector<int> collect_clocks(const std::vector<double> & angles)
{
	std::map<int, int> angles_in_clock;
	for(std::vector<double>::const_iterator it = angles.begin(); it != angles.end(); ++it)
		++ angles_in_clock[get_clock_for_angle(*it)];

	std::vector<int> result;
	for(int i = 1; i <= 12; ++i)
		if (is_enough_points_in_clock(angles_in_clock[i]))
			result.push_back(i);

	return result;
}

bool clocks_contains(std::vector<int> & clocks, int clock)
{
	return std::find(clocks.begin(), clocks.end(), clock) != clocks.end();
}

bool is_single_interval(std::vector<int> & clocks)
{
	int numIntervals = 0;
	bool prevEmpty = clocks.back() != 12;
	for(int i = 1; i <= 12; ++i)
	{
		bool currentEmpty = ! clocks_contains(clocks,i);
		if (currentEmpty != prevEmpty)
			++numIntervals;
		prevEmpty = currentEmpty;
	}
	return numIntervals <= 2;
}

int next_clock(int clock)
{
	return clock == 12 ? 1 : clock + 1;
}

int prev_clock(int clock)
{
	return clock == 1 ? 12 : clock - 1;
}

int get_interval_start(std::vector<int> & clocks)
{
	for(int i = 1; i <= 12; ++i)
		if (clocks_contains(clocks, i) && ! clocks_contains(clocks, prev_clock(i)))
			return i;

	return -1;
}

int get_interval_end(std::vector<int> & clocks)
{
	for(int i = 1; i <= 12; ++i)
		if (clocks_contains(clocks, i) && ! clocks_contains(clocks, next_clock(i)))
			return i;

	return -1;
}

std::vector<std::pair<Shape::ShapeWithAnchors, Probability> > Shape::analyzeCircular(const Image & img)
{
	std::vector<std::pair<Shape::ShapeWithAnchors, Probability> > result;
	Box realBox = img.minimalBox();

	if (! (realBox.is_horizontal() || realBox.is_vertical()) )
	{
		Point center = realBox.center();

		std::vector<double> angles = img.collectAngles(center);

		std::vector<int> clocks = collect_clocks(angles);

		if (!clocks.empty())
		{
			if (clocks.size() == 12)
			{
				Shape shape = Shape::createCircle();
				shape.size = analyzeCircleSize(img);
				result.push_back(std::make_pair(ShapeWithAnchors(shape,anchorsForCircular(shape, img, center)), 100));
			}
			else if (is_single_interval(clocks))
			{
				Shape shape = Shape::createSegment();
				shape.start = get_interval_start(clocks);
				shape.end = get_interval_end(clocks);
				result.push_back(std::make_pair(ShapeWithAnchors(shape,anchorsForCircular(shape, img, center)), 90));
			}
		}
	}

	return result;
}

ShapeSize Shape::analyzeCircleSize(const Image & img)
{
	Box realBox = img.minimalBox();
	Box imageBox = img.box();

	if (imageBox.width() > realBox.width() * 4
		&& imageBox.height() > realBox.height() * 4)
		return ssVerySmall;
	else
		return ssLarge;
}

Anchors Shape::anchorsForCircular(const Shape & shape, const Image & img, Point center)
{
	Anchors result;

	result[ptMiddle] = center;

	for (int clk = 1; clk <= 12; ++clk)
	{
		if (shape.containsClock(clk))
		{
			Point point = findClockPoint(img, center, clk);
			PartType part = segmentStartType(clk);
			result[part] = point;
		}
	}

	return result; //TODO:
}

Point Shape::findClockPoint(const Image & img, Point center, int clk)
{
	Point vectorEnd(0,-1000);
	vectorEnd.rotate((double)clk * M_PI / 6.0);
	return img.closestPoint(center, Vector(Point(0,0), vectorEnd));
}

Shape Shape::createLine()
{
	Shape result;
	result.type = shLine;
	return result;
}

Shape Shape::createLine(Direction dir, ShapeSize sz)
{
	Shape result;
	result.type = shLine;
	result.direction = dir;
	result.size = sz;
	return result;
}

Shape Shape::createCircle()
{
	Shape result;
	result.type = shCircle;
	return result;
}

Shape Shape::createSegment()
{
	Shape result;
	result.type = shSegment;
	return result;
}

std::string Shape::description() const
{
	switch(type)
	{
	case shLine:
		return "line";
	case shCircle:
		return "circle";
	case shSegment:
		{
			std::string result;
			std::ostringstream str(result);
			str << "segment " << start << " to " << end;
			str.flush();
			return str.str();
		}
	default:
		return "unknown";
	}
}

bool Shape::mayBeEqual(const Shape & first, const Shape & second)
{
	if (first.type != second.type)
		return false;

	if (first.type == shLine)
	{
		if (first.direction != second.direction)
			return false;

		return sizeMayBeEqual(first, second);
	}

	if (first.type == shCircle)
	{
		return sizeMayBeEqual(first, second);
	}

	if (first.type == shSegment)
	{
		return sizeMayBeEqual(first, second)
			&& clocksMayBeEqual(first, second);
	}

	return false;
}

bool Shape::sizeMayBeEqual(const Shape & first, const Shape & second)
{
	if (first.size == ssAny || second.size == ssAny)
		return true;

	if (first.size == ssVerySmall)
		return second.size == ssVerySmall || second.size == ssSmall;

	if (first.size == ssLarge)
		return second.size == ssLarge || second.size == ssSmall;

	return true;
}

bool Shape::clocksMayBeEqual(const Shape & first, const Shape & second)
{
	return clocksMayBeEqual(first.start,second.start)
		&& clocksMayBeEqual(first.end,second.end);
}

bool Shape::clocksMayBeEqual(int first, int second)
{
	return abs(first-second) <= Globals::clockThreshold();
}

ImageWithAnchors Shape::draw(const Box & box) const
{
	switch(type)
	{
	case shLine:
		return drawLine(box, direction, size);
	case shCircle:
		return drawSegment(box, size, 12, 12);
	case shSegment:
		return drawSegment(box, size, start, end);;
	default:
		return ImageWithAnchors();
	}
}

ImageWithAnchors Shape::drawLine(const Box & box, Direction direction, ShapeSize size)
{
	ImageWithAnchors result = drawLine(box, direction);
	scale(result, size);
	return result;
}

ImageWithAnchors Shape::drawSegment(const Box & box, ShapeSize size, int start, int end)
{
	ImageWithAnchors result = drawSegment(box, start, end);
	scale(result, size);
	return result;
}

ImageWithAnchors Shape::drawLine(const Box & box, Direction direction)
{
	Point start;
	Point end;
	switch(direction)
	{
	case ddHorizontal:
		start.x = box.topLeft.x;
		start.y = box.center().y;
		end.x = box.bottomRight.x;
		end.y = box.center().y;
		break;
	case ddVertical:
		start.x = box.center().x;
		start.y = box.topLeft.y;
		end.x = box.center().x;
		end.y = box.bottomRight.y;
		break;
	case ddSlash:
		start.x = box.topLeft.x;
		start.y = box.bottomRight.y;
		end.x = box.bottomRight.x;
		end.y = box.topLeft.y;
		break;
	case ddBackslash:
		start = box.topLeft;
		end = box.bottomRight;
		break;
	}

	ImageWithAnchors result;
	drawLine(box, start, end, result);

	anchorizeLine(result);

	return result;
}

void Shape::anchorizeLine(ImageWithAnchors & result)
{
	Image & image = result.image;
	std::map<PartType, Point> & anchors = result.anchors;

	if (image.points().size() > 1)
	{
		Point start = image.points().front();
		Point end = image.points().back();

		if (start.x < end.x)
		{
			anchors[ptLeft] = start;
			anchors[ptRight] = end;
		}
		if (start.x > end.x)
		{
			anchors[ptLeft] = end;
			anchors[ptRight] = start;
		}
		if (start.y < end.y)
		{
			anchors[ptTop] = start;
			anchors[ptBottom] = end;
		}
		if (start.y > end.y)
		{
			anchors[ptTop] = end;
			anchors[ptBottom] = start;
		}

		anchors[ptMiddle] = Box(start,end).center();
	}
}

void Shape::drawLine(const Box & box, Point start, Point end, ImageWithAnchors & result)
{
	result.image.set_box(box);

	Point point = start;

	//Ye olde and faithfule Bresenham

	bool steep = abs(end.y - start.y) > abs(end.x - start.x);
	if (steep)
	{
		std::swap(start.x, start.y);
		std::swap(end.x, end.y);
	}

	if (start.x > end.x)
	{
		std::swap(start.x, end.x);
		std::swap(start.y, end.y);
	}

	int dx = end.x - start.x;
	int dy = abs(end.y - start.y);

	double e = 0;
	double de = (double)dy / (double)dx;

	int ystep = start.y < end.y ? 1 : -1;

	int y = start.y;

	for (int x = start.x; x < end.x; ++x)
	{
		if (steep)
		{
			result.image.add(Point(y,x));
		}
		else
		{
			result.image.add(Point(x,y));
		}

		e += de;

		if (e >= 0.5)
		{
			y += ystep;
			e -= 1.0;
		}
	}
}

ImageWithAnchors Shape::drawSegment(const Box & box, int start, int end)
{
	ImageWithAnchors result;

	std::vector<int> clockwiseSegments = getClockwiseSegments(start, end);

	for (std::vector<int>::iterator it = clockwiseSegments.begin(); it != clockwiseSegments.end(); ++it)
	{
		int segment = *it;
		drawSegment(box, segment, result);
	}

	return result;
}

void Shape::drawSegment(const Box & box, int segment, ImageWithAnchors & result)
{
	Point start = getSegmentStart(box, segment);
	Point end = getSegmentStart(box, segment+1);
	result.image.addBreak();
	drawLine(box, start, end, result);
	result.image.addBreak();
	addSegmentAnchors(segment, start, end, result);
}

void Shape::addSegmentAnchors(int segment, Point start, Point end, ImageWithAnchors & result)
{
	result.anchors[segmentStartType(segment)] = start;
	result.anchors[segmentStartType(segment+1)] = end;
}

PartType Shape::segmentStartType(int segment)
{
	while(segment > 12) 
		segment -= 12;

	if(segment == 0)
		segment = 12;

	switch(segment)
	{
	case 1:
		return pt1;
	case 2:
		return pt2;
	case 3:
		return pt3;
	case 4:
		return pt4;
	case 5:
		return pt5;
	case 6:
		return pt6;
	case 7:
		return pt7;
	case 8:
		return pt8;
	case 9:
		return pt9;
	case 10:
		return pt10;
	case 11:
		return pt11;
	case 12:
		return pt12;
	default:
		return ptNone;
	}
}

Point Shape::getSegmentStart(const Box & box, int segment)
{
	Point result(box.center().x, box.topLeft.y);
	result.rotate(box.center(), (double)segment * M_PI / 6);
	return result;
}

std::vector<int> Shape::getClockwiseSegments(int start, int end)
{
	if (start == 12)
		start = 0;

	while(end < start)
		end += 12;

	std::vector<int> result;

	for(int i = start; i < end; ++i)
		result.push_back(i);

	return result;
}

void Shape::scale(ImageWithAnchors & result, ShapeSize size)
{
	Box originalBox = result.image.box();
	switch(size)
	{
	case ssVerySmall:
		result.scale(0.15,0.15);
		result.transfer((int)(originalBox.width() * 0.375), (int)(originalBox.height() * 0.625));
		break;
	case ssSmall:
		result.scale(0.5,0.5);
		result.transfer(originalBox.width() / 4, originalBox.height() / 4);
		break;
	default:
		result.scale(0.8,0.8);
		result.transfer(originalBox.width() / 10, originalBox.height() / 10);
		break;
	}
}

bool Shape::match(const Shape::ShapeWithAnchors & firstShape, const Shape::ShapeWithAnchors & secondShape, PartType firstPart, PartType secondPart)
{
	if (firstPart == ptNone && secondPart == ptNone)
	{
		return is_no_intersection(firstShape, secondShape);
	}
	else
	{
		Point firstAnchor, secondAnchor;

		if (get_anchor(firstShape, firstPart, firstAnchor) && get_anchor(secondShape, secondPart, secondAnchor))
		{
			return anchors_close(firstShape, secondShape, firstAnchor, secondAnchor);
		}
		else
		{
			return false;
		}
	}
}

bool Shape::get_anchor(const Shape::ShapeWithAnchors & shape, PartType partType, Point & anchor)
{
	Shape::ShapeWithAnchors::second_type::const_iterator it = shape.second.find(partType);
	if (it == shape.second.end())
	{
		return false;
	}
	else
	{
		anchor = it->second;
		return true;
	}
}

bool Shape::is_no_intersection(const Shape::ShapeWithAnchors & firstShape, const Shape::ShapeWithAnchors & secondShape)
{
	//TODO: like, check or something if needed. now, the test cases never get here
	return true;
}

bool Shape::anchors_close(const Shape::ShapeWithAnchors & firstShape, const Shape::ShapeWithAnchors & secondShape, Point firstAnchor, Point secondAnchor)
{
	if ( firstAnchor.dist(secondAnchor) < Globals::defaultImageBox().width() * Globals::anchorTolerance() )
		return true;
	else
		return false;
}