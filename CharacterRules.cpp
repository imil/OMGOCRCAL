#include "stdafx.h"

#include "CharacterRules.h"

std::map<Character, std::vector<Rule> > CharacterRules::m_rules;
Characters CharacterRules::m_numbers;

std::vector<std::pair<Character, Probability> > CharacterRules::recognize(const std::vector<const Image*> & images)
{
	std::vector<Shape::ShapeWithAnchors> shapes;
	for(std::vector<const Image*>::const_iterator it = images.begin(); it != images.end(); ++it)
	{
		Shape::ShapeWithAnchors shape = Shape::mostLikely(**it);
		shapes.push_back(shape);
	}

	std::vector<std::pair<Character, Probability> > result;

	result = filterInvalidShapes(shapes);

	if (result.size() < 2 )
		return result;

	result = filterInvalidJoins(result, shapes);

	if (result.size() < 2 )
		return result;

	result = filterInvalidSize(result, shapes);

	return result;
}

struct filter_character_if_shapes_dont_match_t : public std::unary_function<std::pair<Character, std::vector<Rule> >, void> {
	filter_character_if_shapes_dont_match_t(CharacterRules::CharacterProbabilities & _result, std::vector<Shape::ShapeWithAnchors> & _shapes) 
		: shapes(_shapes), result(_result) {}

	void operator () (const argument_type & characterRules) {
		for(std::vector<Rule>::const_iterator it = characterRules.second.begin(); it != characterRules.second.end(); ++it)
		{
			const Rule & rule = *it;

			if (shapes.size() == rule.shapes.size())
			{
				std::vector<Shape::ShapeWithAnchors> filtered_shapes = shapes;

				for(std::vector<Shape*>::const_iterator it = rule.shapes.begin(); it != rule.shapes.end(); ++it)
				{
					Shape & rule_shape = * * it;

					for(std::vector<Shape::ShapeWithAnchors>::iterator it = filtered_shapes.begin(); it != filtered_shapes.end(); ++it)
					{
						Shape & filtered_shape = it->first;
						if (Shape::mayBeEqual(rule_shape, filtered_shape))
						{
							filtered_shapes.erase(it);
							break;
						}
					}
				}

				if (filtered_shapes.empty())
				{
					result.push_back(std::make_pair(characterRules.first, 1));
				}
			}
		}
	}

	std::vector<Shape::ShapeWithAnchors> & shapes;
	CharacterRules::CharacterProbabilities & result;
};

CharacterRules::CharacterProbabilities CharacterRules::filterInvalidShapes(std::vector<Shape::ShapeWithAnchors> & shapes)
{
	CharacterRules::CharacterProbabilities result;

	filter_character_if_shapes_dont_match_t  filter_character_if_shapes_dont_match(result,shapes);
	std::for_each(rules().begin(), rules().end(), filter_character_if_shapes_dont_match);

	return result;
}

CharacterRules::CharacterProbabilities CharacterRules::filterInvalidJoins(CharacterProbabilities probs, std::vector<Shape::ShapeWithAnchors> & shapes)
{
	CharacterProbabilities result;

	for(CharacterProbabilities::iterator it = probs.begin(); it != probs.end(); ++it)
	{
		CharacterProbability & prob = *it;
		if (mayMatchJoins(prob.first, shapes))
		{
			result.push_back(prob);
		}
	}

	return result;
}

bool CharacterRules::mayMatchJoins(const Character & character, std::vector<Shape::ShapeWithAnchors> & shapes)
{
	std::vector<Rule> & rules = m_rules[character];

	for(std::vector<Rule>::iterator it = rules.begin(); it != rules.end(); ++it)
	{
		Rule & rule = *it;
		if (mayMatchJoins(shapes, rule))
			return true;
	}

	return false;
}

template<class T> std::vector<const T*> vec2ptr(const std::vector<T> & vec)
{
	std::vector<const T*> result;
	for (std::vector<T>::const_iterator it = vec.begin(); it != vec.end(); ++it)
		result.push_back( &(*it) );
	return result;
}

bool CharacterRules::mayMatchJoins(std::vector<Shape::ShapeWithAnchors> & r_shapes, Rule & rule)
{
	std::vector<const Shape::ShapeWithAnchors*> shapes = vec2ptr(r_shapes);

	std::sort(shapes.begin(), shapes.end());

	do
	{
		if (rule.matches(shapes))
			return true;
	}
	while(std::next_permutation(shapes.begin(), shapes.end()));

	return false;
}

bool Rule::matches(const std::vector<const Shape::ShapeWithAnchors*> & shapes) const
{
	for(std::vector<Join*>::const_iterator it = joins.begin(); it != joins.end(); ++it)
	{
		Join* join = *it;
		const Shape::ShapeWithAnchors* first = shapes[shape_index(join->first->shape)];
		const Shape::ShapeWithAnchors* second = shapes[shape_index(join->second->shape)];

		if ( ! Shape::mayBeEqual(first->first, * join->first->shape) )
			return false;

		if ( ! Shape::mayBeEqual(second->first, * join->second->shape) )
			return false;

		if (! join->matches(*first, *second))
			return false;
	}
	return true;
}

size_t Rule::shape_index(Shape* shape) const
{
	return std::find(shapes.begin(), shapes.end(), shape) - shapes.begin();
}

CharacterRules::CharacterProbabilities CharacterRules::filterInvalidSize(CharacterProbabilities probs, std::vector<Shape::ShapeWithAnchors> & shapes)
{
	return probs;
}

std::vector<std::pair<Character, Probability> > CharacterRules::recognize(const std::vector<Image> & images)
{
	return CharacterRules::recognize(vec2ptr(images));
}

Character CharacterRules::recognizeSingle(const std::vector<Image> & images)
{
	CharacterProbabilities probs = recognize(images);
	if(probs.size() == 1)
		return probs.front().first;
	else
		return Character();
}

Rule & CharacterRules::rule_for(const Character & character)
{
	static bool sranded = false;
	if (!sranded)
	{
		srand(clock());
		sranded = true;
	}

	std::vector<Rule> & rules = m_rules[character];

	if (rules.empty())
		return * new Rule(); //mwahaha

	return rules[rand() % rules.size()];
}