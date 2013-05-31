#pragma once

#include "Part.h"
#include "Shape.h"

//Join represents proximity of shape parts
//(or lack thereof)
struct Join
{
	static Join* create() {
		return new Join(NULL,NULL); }
	Join(Part* _first, Part* _second) : first(_first), second(_second) {}
	~Join(void) {}

	PartType part_type(Shape* shape) const {
		if (first->shape == shape)
			return first->type;
		else if (second->shape == shape)
			return second->type;
		else
			return ptNone;
	}

	bool matches_shapes(Shape* first_shape, Shape* second_shape) const {
		return (first->shape == first_shape && second->shape == second_shape)
			|| (second->shape == first_shape && first->shape == second_shape);
	}

	//Test shapes: do they match this join?
	bool matches(const Shape::ShapeWithAnchors & first, const Shape::ShapeWithAnchors & second) const;

	Part* first;
	Part* second;
}; 
