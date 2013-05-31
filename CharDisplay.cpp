#include "StdAfx.h"

#include "Globals.h"
#include "CharDisplay.h"

CharDisplay::CharDisplay(bool fit) : m_fit(fit), charBox(Globals::defaultImageBox())
{
}

CharDisplay::~CharDisplay(void)
{
}

void CharDisplay::updateCharBox()
{
	charBox = Globals::defaultImageBox();
}

void CharDisplay::add(Character character)
{
	m_characters.push_back(character);
	m_images.push_back(character.draw(charBox));
}

void CharDisplay::assign(const std::vector<Character> & characters)
{
	clear();
	for(std::vector<Character>::const_iterator it = characters.begin(); it != characters.end(); ++it)
		add(*it);
}

void CharDisplay::clear()
{
	m_characters.clear();
	m_images.clear();
}

void CharDisplay::draw(HDC hdc, RECT rect) const
{
	if(m_fit)
	{
		drawRect(hdc, rect, dsResultBorder);
	}

	Box currentBox = fit_if_needed(charBox, rect);

	std::vector<int> indices = displayedIndices(currentBox,rect);

	for(std::vector<int>::iterator it = indices.begin(); it != indices.end(); ++it )
	{
		int i = *it;

		const CompositeImage & image = m_images[i];
		image.draw(hdc, realBox(rect, currentBox));
		currentBox = shiftBox(currentBox, rect);
	}
}

Box CharDisplay::fit_if_needed(Box box, RECT rect) const
{
	if(m_fit && !m_images.empty())
	{
		while(displayedIndices(box, rect).empty() || displayedIndices(box, rect).front() != 0)
		{
			box.scale(0.9,0.9);
		}
	}
	return box;
}

std::vector<int> CharDisplay::displayedIndices(Box currentBox, RECT _rect) const
{
	Box rect = _rect;
	int max_rows = rect.height() / currentBox.height();
	int max_columns = rect.width() / currentBox.width();

	int max_cells = max_rows * max_columns;

	int first_cell = 0;
	while((int)m_images.size() - first_cell > max_cells)
		first_cell += 1;

	std::vector<int> result;
	for (int i = first_cell; i < (int)m_images.size(); ++i)
		result.push_back(i);
	return result;
}
