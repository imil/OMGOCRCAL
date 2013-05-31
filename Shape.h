#pragma once

#include "ShapeTypes.h"
#include "Probability.h"
#include "Image.h"
#include "Point.h"
#include "Anchor.h"
#include "ImageWithAnchor.h"

//A geometrical entity (line, circle or segment) forming the characters
//A violation of all OO Design rules with its switches and public members, but works.
class Shape
{
public:
	//Shape parameters
	ShapeType type;
	ShapeSize size;
	Direction direction;
	int start; //measured in clocks
	int end;   //(1 to 12)

	typedef std::pair<Shape, Anchors> ShapeWithAnchors;
	typedef std::vector<std::pair<ShapeWithAnchors, Probability> > AllVariants;

	//Recognize the image. May return several variants
	static AllVariants analyze(const Image & img);

	//Returns most probable shape
	static ShapeWithAnchors mostLikely(const Image & img);

	//Test likeness of shapes. Returns true for completely equal shapes,
	//but also for those that differ slightly, for example
	//segment 9 to 3 is considered equal to segment 10 to 5.
	//Tolerance values are stored in Globals.h
	static bool mayBeEqual(const Shape & first, const Shape & second);

	//Test if the specified parts of two shapes are close enough
	static bool match(const Shape::ShapeWithAnchors & firstShape, const Shape::ShapeWithAnchors & secondShape, PartType firstPart, PartType secondPart);

	Shape(void);
	virtual ~Shape(void);

	//Draw the shape. Image is fit inside given box
	ImageWithAnchors draw(const Box & box) const;

	std::string description() const;

	static Shape createLine();
	static Shape createLine(Direction dir, ShapeSize sz);
	static Shape createCircle();
	static Shape createSegment();

protected:
	static ImageWithAnchors drawLine(const Box & box, Direction direction, ShapeSize size);
	static ImageWithAnchors drawSegment(const Box & box, ShapeSize size, int start, int end);
	static ImageWithAnchors drawLine(const Box & box, Direction direction);
	static void drawLine(const Box & box, Point start, Point end, ImageWithAnchors & result);
	static ImageWithAnchors drawSegment(const Box & box, int start, int end);
	static void drawSegment(const Box & box, int segment, ImageWithAnchors & result);
	static void scale(ImageWithAnchors & result, ShapeSize size);
	static void anchorizeLine(ImageWithAnchors & result);
	static void addSegmentAnchors(int segment, Point start, Point end, ImageWithAnchors & result);

	static std::vector<std::pair<ShapeWithAnchors, Probability> > analyzeLine(const Image & img);
	static Direction analyzeLineDirection(const Image & img);
	static std::vector<std::pair<ShapeWithAnchors, Probability> > analyzeCircular(const Image & img);
	static std::vector<int> getClockwiseSegments(int start, int end);
	static Point getSegmentStart(const Box & box, int segment);
	static PartType segmentStartType(int segment);
	static Anchors anchorsForLine(const Shape & shape, const Image & img);
	static Anchors anchorsForCircular(const Shape & shape, const Image & img, Point center);
	//Applies only to circles and segments
	bool containsClock(int clk) const;

	static Point topPoint(const Image & img);
	static Point rightPoint(const Image & img);
	static Point leftPoint(const Image & img);
	static Point bottomPoint(const Image & img);
	static Point middlePoint(const Image & img);

	static Point findClockPoint(const Image & img, Point center, int clk);
	static ShapeSize analyzeCircleSize(const Image & img);

	static bool sizeMayBeEqual(const Shape & first, const Shape & second);
	static bool clocksMayBeEqual(const Shape & first, const Shape & second);
	static bool clocksMayBeEqual(int first, int second);

	static bool is_no_intersection(const Shape::ShapeWithAnchors & firstShape, const Shape::ShapeWithAnchors & secondShape);
	static bool get_anchor(const Shape::ShapeWithAnchors & shape, PartType partType, Point & anchor);
	static bool anchors_close(const Shape::ShapeWithAnchors & firstShape, const Shape::ShapeWithAnchors & secondShape, Point firstAnchor, Point secondAnchor);
};
