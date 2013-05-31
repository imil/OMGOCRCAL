#include "StdAfx.h"

#include "Keypad.h"

#include "Character.h"
#include "Globals.h"

Keypad::Keypad()
{
	images[pair(0,0)] = image("Seven");
	images[pair(1,0)] = image("Eight");
	images[pair(2,0)] = image("Nine");
	images[pair(3,0)] = image("Plus");
	images[pair(0,1)] = image("Four");
	images[pair(1,1)] = image("Five");
	images[pair(2,1)] = image("Six");
	images[pair(3,1)] = image("Minus");
	images[pair(0,2)] = image("One");
	images[pair(1,2)] = image("Two");
	images[pair(2,2)] = image("Three");
	images[pair(3,2)] = image("Multiply");
	images[pair(0,3)] = image("Zero");
	images[pair(1,3)] = image("Dot");
	images[pair(2,3)] = image("Equals");
	images[pair(3,3)] = image("Divide");
}

CompositeImage Keypad::image(const std::string & char_name)
{
	Character character(char_name);
	return character.draw(Globals::defaultImageBox());
}

Keypad::~Keypad(void)
{
}

void Keypad::draw(HDC hdc, RECT rect) const
{
	for (int x = 0; x < 4; ++x)
		for (int y = 0; y < 4; ++y)
			drawCell(hdc, rect, x, y);

	drawBorders(hdc, rect);
}

void Keypad::drawCell(HDC hdc, RECT totalRect, int x, int y) const
{
	drawRect(hdc, subrect(totalRect, x, y, false), dsCalculatorCellBorder);

	const CompositeImage & image = images.find(pair(x,y))->second;
	image.draw(hdc, subrect(totalRect, x, y, true));
}

void Keypad::drawMarked(HDC hdc, RECT rect, const Character & character) const
{
	drawMarked(hdc, rect, character, dsCalculatorMarkedCell);
}

void Keypad::drawMarked(HDC hdc, RECT rect, const Character & character, DrawStyle style) const
{
	int x,y;
	if (findChar(character, x, y))
	{
		drawRect(hdc, subrect(rect, x, y, false), style);
	}
}

void Keypad::drawMarked(HDC hdc, RECT rect, const std::vector<Character> & characters) const
{
	DrawStyle style = characters.size() == 1 ? dsCalculatorMarkedCell : dsCalculatorVariantCell;
	for (std::vector<Character>::const_iterator it = characters.begin(); it != characters.end(); ++it)
	{
		const Character & ch = *it;
		drawMarked(hdc, rect, ch, style);
	}
}

bool Keypad::findChar(const Character & character, int & x, int & y) const
{
	const std::string & name = character.m_name;
	if (name == "Seven")
	{
		x = 0;
		y = 0;
		return true;
	}
	if (name == "Eight")
	{
		x = 1;
		y = 0;
		return true;
	}
	if (name == "Nine")
	{
		x = 2;
		y = 0;
		return true;
	}
	if (name == "Plus")
	{
		x = 3;
		y = 0;
		return true;
	}
	if (name == "Four")
	{
		x = 0;
		y = 1;
		return true;
	}
	if (name == "Five")
	{
		x = 1;
		y = 1;
		return true;
	}
	if (name == "Six")
	{
		x = 2;
		y = 1;
		return true;
	}
	if (name == "Minus")
	{
		x = 3;
		y = 1;
		return true;
	}
	if (name == "One")
	{
		x = 0;
		y = 2;
		return true;
	}
	if (name == "Two")
	{
		x = 1;
		y = 2;
		return true;
	}
	if (name == "Three")
	{
		x = 2;
		y = 2;
		return true;
	}
	if (name == "Multiply")
	{
		x = 3;
		y = 2;
		return true;
	}
	if (name == "Zero")
	{
		x = 0;
		y = 3;
		return true;
	}
	if (name == "Dot")
	{
		x = 1;
		y = 3;
		return true;
	}
	if (name == "Equals")
	{
		x = 2;
		y = 3;
		return true;
	}
	if (name == "Divide")
	{
		x = 3;
		y = 3;
		return true;
	}


	return false;
}

int pad_if(bool pad)
{
	return pad ? 2 : 0;
}

RECT Keypad::subrect(RECT totalRect, int x, int y, bool pad) const
{
	int width = (totalRect.right - totalRect.left) / 4;
	int height = (totalRect.bottom - totalRect.top) / 4;
	int left = totalRect.left + x * width;
	int top = totalRect.top + y * height;

	RECT result;

	result.left = left + pad_if(pad);
	result.top = top + pad_if(pad);
	result.right = left + width - pad_if(pad);
	result.bottom = top + height - pad_if(pad);

	return result;
}

void Keypad::drawBorders(HDC hdc, RECT rect) const
{
	drawRect(hdc, rect, dsCalculatorBorder);
}

const CompositeImage* Keypad::locate(Point point, Box box) const
{
	double dx = point.x - box.topLeft.x;
	double dy = point.y - box.topLeft.y;

	double width = box.width();
	double height = box.height();

	double xpart = dx * 4 / width;
	double ypart = dy * 4 / height;

	int xind = (int)xpart;
	int yind = (int)ypart;

	return & images.find(pair(xind,yind))->second;
}