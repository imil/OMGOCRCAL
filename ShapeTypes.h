#pragma once

//This file contains definitions of shape parameters

//Main shape type
enum ShapeType
{
	shNone,
	shLine,
	shCircle,
	shSegment,
};

//Shape size. Note the usage of synonims.
//During recognition only difference between VerySmall and Large is taken into account.
//During output, however, all sizes are used
enum ShapeSize
{
	ssAny = 0,
	ssSmall = 1,
	ssShort = 1,
	ssLarge = 2,
	ssLong = 2,
	ssVerySmall = 3,
	ssVeryShort = 3,
};

//Direction of line
enum Direction
{
	ddAny,
	ddHorizontal,
	ddVertical,
	ddSlash,
	ddBackslash,
};

//Parts of shapes. Not every value applies to each shape.
//For example, pt1 - pt12 are used only in circular shapes 
//and ptTop is not applicable to horizontal lines
enum PartType
{
	ptAny,
	ptNone,
	ptMiddle,
	ptTop,
	ptBottom,
	ptLeft,
	ptRight,
	pt1,
	pt2,
	pt3,
	pt4,
	pt5,
	pt6,
	pt7,
	pt8,
	pt9,
	pt10,
	pt11,
	pt12,

	ptAbove,
	ptBelow,
	ptLefter,
	ptRighter,
};

//Returns list of part types that are considered similar to given one.
//The same part type is always returned.
//For pt1 - pt12 two adjacent clocks from each side are also returned
std::vector<PartType> similar_parts(PartType type);
