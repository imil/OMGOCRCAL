#pragma once

#include "Image.h"
#include "Character.h"
#include "Probability.h"
#include "Join.h"

//Rule that describes a character
struct Rule
{
	std::vector<Shape*> shapes;
	std::vector<Join*> joins;

	bool matches(const std::vector<const Shape::ShapeWithAnchors*> & shapes) const;
	size_t shape_index(Shape* shape) const;
};

//Set of all character descriptions
class CharacterRules
{
public:
	typedef std::pair<Character, Probability> CharacterProbability;
	typedef std::vector<CharacterProbability> CharacterProbabilities;

	//Actual recognition functions
	static CharacterProbabilities recognize(const std::vector<const Image*> & images);
	static CharacterProbabilities recognize(const std::vector<Image> & images);
	static Character recognizeSingle(const std::vector<Image> & images);

	static std::map<Character, std::vector<Rule> > & rules() {
		return m_rules;
	}

	static Characters & numbers() {
		return m_numbers;
	}

	//Returns random (if >1 possible) rule for the given character
	static Rule & rule_for(const Character & character);

private:
	static std::map<Character, std::vector<Rule> > m_rules;
	static Characters m_numbers;
	CharacterRules() {}

	static CharacterProbabilities filterInvalidShapes(std::vector<Shape::ShapeWithAnchors> & shapes);
	static CharacterProbabilities filterInvalidJoins(CharacterProbabilities probs, std::vector<Shape::ShapeWithAnchors> & shapes);
	static CharacterProbabilities filterInvalidSize(CharacterProbabilities probs, std::vector<Shape::ShapeWithAnchors> & shapes);

	static bool mayMatchJoins(const Character & character, std::vector<Shape::ShapeWithAnchors> & shapes);
	static bool mayMatchJoins(std::vector<Shape::ShapeWithAnchors> & shapes, Rule & rule);
};
