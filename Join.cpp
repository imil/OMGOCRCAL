#include "stdafx.h"

#include "Join.h"

bool Join::matches(const Shape::ShapeWithAnchors & firstShape, const Shape::ShapeWithAnchors & secondShape) const
{
	PartType firstPartType = first->type;
	PartType secondPartType = second->type;

	std::vector<PartType> firstTypes = similar_parts(firstPartType);
	std::vector<PartType> secondTypes = similar_parts(secondPartType);

	for (std::vector<PartType>::iterator it = firstTypes.begin(); it != firstTypes.end(); ++it)
	{
		PartType firstType = *it;
		for (std::vector<PartType>::iterator it = secondTypes.begin(); it != secondTypes.end(); ++it)
		{
			PartType secondType = *it;

			if ( Shape::match(firstShape, secondShape, firstType, secondType) )
				return true;
		}
	}

	return false;
}