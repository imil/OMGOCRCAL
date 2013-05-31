#pragma once

#include "Visible.h"
#include "Image.h"
#include "Character.h"

//Like, a keypad
class Keypad : public Visible
{
public:
	Keypad();
	~Keypad(void);

	void draw(HDC hdc, RECT rect) const;
	void drawMarked(HDC hdc, RECT rect, const Character & character) const;
	void drawMarked(HDC hdc, RECT rect, const std::vector<Character> & characters) const;

	//Returns the image on clicked button
	const CompositeImage* locate(Point point, Box box) const;
	
private:
	static std::pair<int, int> pair(int x, int y) {
		return std::make_pair(x,y);
	}
	static CompositeImage image(const std::string & character);
	std::map<std::pair<int, int>, CompositeImage> images;
	void drawCell(HDC hdc, RECT totalRect, int x, int y) const;
	RECT subrect(RECT totalRect, int x, int y, bool pad) const;
	void drawBorders(HDC hdc, RECT rect) const;
	bool findChar(const Character & character, int & x, int & y) const;
	void drawMarked(HDC hdc, RECT rect, const Character & character, DrawStyle style) const;
};
