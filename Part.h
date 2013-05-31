#pragma once

#include "ShapeTypes.h"

//Part of the shape, used in joins
struct Part
{
public:
	class Shape* shape;
	PartType type;

	Part(class Shape* _shape, PartType _type) :shape(_shape), type(_type) {}
	~Part(void);
};
