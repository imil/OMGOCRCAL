#include "stdafx.h"

#include "Character.h"
#include "CharacterRules.h"

Character::Character(const std::string & name)
	:m_name(name)
{
}

Character::~Character(void)
{
}

CompositeImage Character::draw(const Box & box) const
{
	Rule & rule = CharacterRules::rule_for(*this);

	std::vector<ImageWithAnchors> shape_images = drawShapes(box, rule.shapes);

	return ImageWithAnchors::join(shape_images, box, rule);
}

std::vector<ImageWithAnchors> Character::drawShapes(const Box & box, const std::vector<Shape*> & shapes) const
{
	std::vector<ImageWithAnchors> result;
	for (std::vector<Shape*>::const_iterator it = shapes.begin(); it != shapes.end(); ++it)
	{
		Shape & shape = **it;
		result.push_back(shape.draw(box));
	}
	return result;
}
