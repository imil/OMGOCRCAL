#include "stdafx.h"

#include "ImageWithAnchor.h"
#include "CharacterRules.h"

void ImageWithAnchors::scale(double dx, double dy)
{
	image.scale(dx,dy);
	for(std::map<PartType, Point>::iterator it = anchors.begin(); it != anchors.end(); ++it)
		it->second.scale(dx,dy);
}

void ImageWithAnchors::transfer(int dx, int dy)
{
	image.transfer(dx,dy);
	for(std::map<PartType, Point>::iterator it = anchors.begin(); it != anchors.end(); ++it)
		it->second.transfer(dx,dy);
}

CompositeImage ImageWithAnchors::join(std::vector<ImageWithAnchors> & shape_images, const Box & box, const Rule & rule)
{
	//ImageWithAnchors & firstImage = shape_images.front();
	for(unsigned int i = 1; i < shape_images.size(); ++i)
	{
		ImageWithAnchors & iwa = shape_images[i];
		for (unsigned int j = 0; j < i; ++j)
		{
			const ImageWithAnchors & fixedImage = shape_images[j];
			adjust(shape_images, i, j, rule);
		}
	}

	CompositeImage result;
	collect(shape_images, result);
	result.fit(box.shrunk_by(4));
	return result;
}

bool is_to_side(PartType partType)
{
	switch(partType)
	{
	case ptNone:
	case ptAbove:
	case ptBelow:
	case ptLefter:
	case ptRighter:
		return true;
	default:
		return false;
	}
}

int get_dx(PartType fixedPart, const ImageWithAnchors & fixedImage, const ImageWithAnchors & adjustedImage)
{
	switch(fixedPart)
	{
	case ptLefter:
		return fixedImage.image.box().width() / 5;
	case ptRighter:
		return - fixedImage.image.box().width() / 5;
	default:
		return 0;
	}
}

int get_dy(PartType fixedPart, const ImageWithAnchors & fixedImage, const ImageWithAnchors & adjustedImage)
{
	switch(fixedPart)
	{
	case ptAbove:
	case ptNone:
		return fixedImage.image.box().height() / 5;
	case ptBelow:
		return - fixedImage.image.box().height() / 5;
	default:
		return 0;
	}
}

void ImageWithAnchors::adjust(std::vector<ImageWithAnchors> & shape_images, int adjusted_index, int fixed_index, const Rule & rule)
{
	ImageWithAnchors & image = shape_images[adjusted_index];
	const ImageWithAnchors & fixedImage = shape_images[fixed_index];
	for(std::vector<Join*>::const_iterator it = rule.joins.begin(); it != rule.joins.end(); ++it)
	{
		Join & join = **it;
		if (join.matches_shapes(rule.shapes[adjusted_index], rule.shapes[fixed_index]))
		{
			PartType fixedPart = join.part_type(rule.shapes[fixed_index]);
			PartType adjustedPart = join.part_type(rule.shapes[adjusted_index]);

			int dx, dy;
			if (is_to_side(fixedPart)) //appears only in =
			{
				dx = get_dx(fixedPart, fixedImage, image);
				dy = get_dy(fixedPart, fixedImage, image);
			}
			else
			{
				const Point & fixedPoint = fixedImage.anchors.find(fixedPart)->second;
				const Point & adjustedPoint = image.anchors.find(adjustedPart)->second;

				dx = fixedPoint.x - adjustedPoint.x;
				dy = fixedPoint.y - adjustedPoint.y;
			}

			image.transfer(dx, dy);
		}
	}
}

void ImageWithAnchors::collect(const std::vector<ImageWithAnchors> & images, CompositeImage & result)
{
	for(std::vector<ImageWithAnchors>::const_iterator it = images.begin(); it != images.end(); ++it)
	{
		const Image & image = it->image;
		result.add(image);
	}
}
