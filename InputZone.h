#pragma once

#include "Visible.h"
#include "Character.h"
#include "Image.h"

//Main input zone: consists of already entered images and input areas
class InputZone : public Visible
{
public:
	InputZone(void);
	~InputZone(void);

	void draw(HDC hdc, RECT rect) const;
	Box charBox;
	void updateCharBox();

	void clear();

	//handle manual input
	void drag(const std::vector<Point> & points, RECT rect);
	//handle keypad input
	void add(const CompositeImage & image);

	//All recognized characters
	std::vector<Character> characters() const;

	//Character(s) recognized in the input box
	const std::vector<Character> & currentChars() const {
		return m_recognized_characters;
	}

protected:
	bool is_in_next_area(const std::vector<Point> & points, RECT rect) const;
	bool is_in_current_input_area(const std::vector<Point> & points, RECT rect) const;
	Box currentInputBox(RECT rect) const;
	Box nextInputBox(RECT rect) const;
	void step();
	void recognize();
	bool is_recognized(size_t index) const;
	std::vector<int> displayedIndices(RECT rect) const;
	void dragInInputZone(const std::vector<Point> & points, Box inputBox, RECT rect);

	std::vector<CompositeImage> m_images;
	std::vector<Character> m_characters;
	std::vector<Character> m_recognized_characters;
	std::set<size_t> m_unrecognized_images;
};
