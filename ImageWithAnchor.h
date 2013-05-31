#pragma once

#include "Part.h"
#include "Image.h"
#include "Anchor.h"

//Image along with anchor data
struct ImageWithAnchors
{
	Image image;
	Anchors anchors;

	static CompositeImage join(std::vector<ImageWithAnchors> & shape_images, const Box & box, const struct Rule & rule);

	void scale(double dx, double dy);
	void transfer(int dx, int dy);

private:
	static void adjust(std::vector<ImageWithAnchors> & shape_images, int adjusted_index, int fixed_index, const Rule & rule);
	static void collect(const std::vector<ImageWithAnchors> & images, CompositeImage & result);
};
