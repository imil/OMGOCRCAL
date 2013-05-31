#include "StdAfx.h"

#include "Globals.h"
#include "InputZone.h"
#include "CharacterRules.h"

InputZone::InputZone(void) : charBox(Globals::defaultImageBox())
{
}

InputZone::~InputZone(void)
{
}

void InputZone::updateCharBox()
{
	charBox = Globals::defaultImageBox();
}

void InputZone::draw(HDC hdc, RECT rect) const
{
	Box currentBox = charBox;

	std::vector<int> indices = displayedIndices(rect);

	for(std::vector<int>::iterator it = indices.begin(); it != indices.end(); ++it )
	{
		int i = *it;

		const CompositeImage & image = m_images[i];

		drawRect(hdc, realBox(rect, currentBox).shrunk_by(1), is_recognized(i) ? dsInputZoneCell : dsInputZoneInvalidCell);
		image.draw(hdc, realBox(rect, currentBox));

		currentBox = shiftBox(currentBox, rect);
	}
	drawRect(hdc, realBox(rect,currentInputBox(rect)).shrunk_by(1), dsInputZoneCurrent);
	drawRect(hdc, realBox(rect,nextInputBox(rect)).shrunk_by(1), dsInputZoneNext);
}

std::vector<int> InputZone::displayedIndices(RECT _rect) const
{
	Box rect = _rect;
	int max_rows = rect.height() / charBox.height();
	int max_columns = rect.width() / charBox.width();

	int max_cells = max_rows * max_columns;

	int first_cell = 0;
	while((int)m_images.size() + 1 - first_cell > max_cells)
		first_cell += max_columns;

	std::vector<int> result;
	for (int i = first_cell; i < (int)m_images.size(); ++i)
		result.push_back(i);
	return result;
}

bool InputZone::is_recognized(size_t index) const
{
	return m_unrecognized_images.find(index) == m_unrecognized_images.end();
}

void InputZone::clear()
{
	m_images.clear();
	m_characters.clear();
	m_recognized_characters.clear();
	m_unrecognized_images.clear();
}

void InputZone::add(const CompositeImage & image)
{
	if (!m_images.empty() && !m_images.back().empty())
	{
		step();
		m_images.pop_back();
	}
	m_images.push_back(image);
	recognize();
}

void InputZone::drag(const std::vector<Point> & points, RECT rect)
{
	if (is_in_current_input_area(points, rect))
	{
		dragInInputZone(points, currentInputBox(rect), rect);
	}
	else if (is_in_next_area(points, rect))
	{
		Box inputBox = nextInputBox(rect);
		step();
		dragInInputZone(points, inputBox, rect);
	}
}

void InputZone::dragInInputZone(const std::vector<Point> & points, Box inputBox, RECT rect)
{
	m_images.back().add(Image(realBox(rect,inputBox), points));
	recognize();
}

void InputZone::step()
{
	if(m_recognized_characters.size() == 1)
		m_characters.push_back(m_recognized_characters.front());
	else
		m_unrecognized_images.insert(m_images.size()-1);

	m_recognized_characters.clear();

	m_images.push_back(CompositeImage());
}

void InputZone::recognize()
{
	m_recognized_characters.clear();

	CharacterRules::CharacterProbabilities probs = CharacterRules::recognize(m_images.back());

	for (CharacterRules::CharacterProbabilities::iterator it = probs.begin(); it != probs.end(); ++it)
	{
		CharacterRules::CharacterProbability & prob = *it;
		Character & ch = prob.first;
		m_recognized_characters.push_back(ch);
	}
}

bool InputZone::is_in_next_area(const std::vector<Point> & points, RECT rect) const
{
	Box nextBox = charBox;

	std::vector<int> indices = displayedIndices(rect);

	for(std::vector<int>::iterator it = indices.begin(); it != indices.end(); ++it )
	{
		nextBox = shiftBox(nextBox, rect);
	}

	//return nextBox.contains(points);
	return realBox(rect,nextBox).contains(points);
}

bool InputZone::is_in_current_input_area(const std::vector<Point> & points, RECT rect) const
{
	if (m_images.empty())
		return false;

	//return currentInputBox(rect).contains(points);
	return realBox(rect,currentInputBox(rect)).contains(points);
}

Box InputZone::currentInputBox(RECT rect) const
{
	Box nextBox = charBox;
	Box prevBox;

	std::vector<int> indices = displayedIndices(rect);

	for(std::vector<int>::iterator it = indices.begin(); it != indices.end(); ++it )
	{
		prevBox = nextBox;
		nextBox = shiftBox(nextBox, rect);
	}

	return prevBox;
}

Box InputZone::nextInputBox(RECT rect) const
{
	Box nextBox = charBox;
	Box prevBox;

	std::vector<int> indices = displayedIndices(rect);

	for(std::vector<int>::iterator it = indices.begin(); it != indices.end(); ++it )
	{
		prevBox = nextBox;
		nextBox = shiftBox(nextBox, rect);
	}

	return nextBox;
}

std::vector<Character> InputZone::characters() const
{
	std::vector<Character> result;

	for(std::vector<Character>::const_iterator it = m_characters.begin(); it != m_characters.end(); ++it)
	{
		const Character & ch = *it;
		if (! ch.m_name.empty() )
		{
			result.push_back(ch);
		}
	}

	if(m_recognized_characters.size() == 1)
	{
		result.push_back(m_recognized_characters.front());
	}

	return result;
}