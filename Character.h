#pragma once

#include "Join.h"
#include "Shape.h"

//Characters represent all digits and special characters
class Character
{
public:
	Character() {}
	Character(const std::string & name);

	CompositeImage draw(const Box & box) const;

	~Character(void);

	const std::string & name() const {
		return m_name; }

	bool empty() const {
		return m_name.empty(); }

public:
	//Not a good thing to make it public... but was needed somewhere
	std::string m_name;

private:
	std::vector<ImageWithAnchors> drawShapes(const Box & box, const std::vector<Shape*> & shapes) const;
};

//Analog of string : may represent numbers, expressions, etc
typedef std::vector<Character> Characters;



inline bool operator < (const Character & first, const Character & second) {
	return first.m_name < second.m_name; }

inline bool operator == (const Character & first, const Character & second) {
	return first.m_name == second.m_name; }

inline bool operator != (const Character & first, const Character & second) {
	return ! (first == second);
}