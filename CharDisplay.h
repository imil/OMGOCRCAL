#pragma once

#include "Visible.h"
#include "Character.h"
#include "Point.h"
#include "Image.h"

//Static display of character string
class CharDisplay : public Visible
{
public:
	//if fit == true, the images are shrunk to fit in the display rectangle,
	//otherwise only the last are displayed
	CharDisplay(bool fit);
	virtual ~CharDisplay(void);

	void add(Character character);
	void assign(const std::vector<Character> & characters);
	void clear();

	const std::vector<Character> & characters() {
		return m_characters; }
	
	void draw(HDC hdc, RECT rect) const;

	Box charBox;
	void updateCharBox();

protected:
	std::vector<int> displayedIndices(Box currentBox, RECT _rect) const;
	Box fit_if_needed(Box box, RECT rect) const;

private:
	std::vector<Character> m_characters;
	std::vector<CompositeImage> m_images;
	bool m_fit;
};
