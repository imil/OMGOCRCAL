#include "stdafx.h"

#include "Part.h"

Part::~Part(void)
{
}

std::vector<PartType> similar_parts(PartType type)
{
	std::vector<PartType> result;
	result.push_back(type);

	switch(type)
	{
	case pt1:
		result.push_back(pt11);
		result.push_back(pt12);
		result.push_back(pt2);
		result.push_back(pt3);
		break;
	case pt2:
		result.push_back(pt12);
		result.push_back(pt1);
		result.push_back(pt3);
		result.push_back(pt4);
		break;
	case pt3:
		result.push_back(pt1);
		result.push_back(pt2);
		result.push_back(pt4);
		result.push_back(pt5);
		break;
	case pt4:
		result.push_back(pt2);
		result.push_back(pt3);
		result.push_back(pt5);
		result.push_back(pt6);
		break;
	case pt5:
		result.push_back(pt3);
		result.push_back(pt4);
		result.push_back(pt6);
		result.push_back(pt7);
		break;
	case pt6:
		result.push_back(pt4);
		result.push_back(pt5);
		result.push_back(pt7);
		result.push_back(pt8);
		break;
	case pt7:
		result.push_back(pt5);
		result.push_back(pt6);
		result.push_back(pt8);
		result.push_back(pt9);
		break;
	case pt8:
		result.push_back(pt6);
		result.push_back(pt7);
		result.push_back(pt9);
		result.push_back(pt10);
		break;
	case pt9:
		result.push_back(pt7);
		result.push_back(pt8);
		result.push_back(pt10);
		result.push_back(pt11);
		break;
	case pt10:
		result.push_back(pt8);
		result.push_back(pt9);
		result.push_back(pt11);
		result.push_back(pt12);
		break;
	case pt11:
		result.push_back(pt9);
		result.push_back(pt10);
		result.push_back(pt12);
		result.push_back(pt1);
		break;
	case pt12:
		result.push_back(pt10);
		result.push_back(pt11);
		result.push_back(pt1);
		result.push_back(pt2);
		break;
	}

	return result;
}