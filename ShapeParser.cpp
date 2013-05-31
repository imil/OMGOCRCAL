#include "stdafx.h"

#include "ShapeParser.h"


ShapeParser::ShapeParser(void)
{
}

ShapeParser::~ShapeParser(void)
{
}

void ShapeParser::parse(const std::string & fileName)
{
	CharacterRules::rules().clear();
	CharacterRules::numbers().clear();
	parse(tokenizeFile(fileName));

	if (CharacterRules::numbers().empty())
		COMPATIBILITY_V1_fillDefaultNumericSequence();
}

void ShapeParser::parse(std::vector < std::vector< std::string> > & tokens)
{
	Character currentChar;

	for (std::vector < std::vector< std::string> >::iterator it = tokens.begin(); it != tokens.end(); ++it)
	{
		std::vector< std::string> & currentLine = *it;

		if (!currentLine.empty())
		{
			parseNumericSequenceDefinition(currentChar, currentLine)
				||
			parseNumericSequence(currentChar, currentLine)
				||
			parseChardef(currentChar, currentLine)
				||
			parseAlterRule(currentChar, currentLine)
				||
			parseShape(currentChar, currentLine)
				||
			parseJoin(currentChar, currentLine)
				||
			failUnknownLine(currentLine);
		}
	}
}

bool ShapeParser::parseChardef(Character & character, std::vector< std::string> & line)
{
	if (line.size() == 2 && line[1] == "is")
	{
		character = line[0];
		CharacterRules::rules()[character].push_back(Rule());
		return true;
	}
	return false;
}

bool ShapeParser::parseNumericSequenceDefinition(Character & character, TokenString & line)
{
	if (line.size() == 2 
		&& line[0] == "Numbers"
		&& line[1] == "are" )
	{
		character = numSeqTag();
		return true;
	}
	return false;
}

bool ShapeParser::parseNumericSequence(Character & character, TokenString & line)
{
	if (character == numSeqTag())
	{
		for (TokenString::iterator it = line.begin(); it != line.end(); ++it)
		{
			std::string & name = *it;
			CharacterRules::numbers().push_back(Character(name));
		}
		return true;
	}
	return false;
}

Character ShapeParser::numSeqTag()
{
	return Character("____NUM###SEq!@TAg_RESERVED!!!!don'T^Use!!!!EVAR!!!_____");
}

bool ShapeParser::parseAlterRule(Character & character, std::vector< std::string> & line)
{
	if (line.size() == 1)
	{
		if (line[0] == "either")
		{
			return true;
		}
		else if (line[0] == "or")
		{
			CharacterRules::rules()[character].push_back(Rule());
			return true;
		}
	}
	return false;
}

bool ShapeParser::parseShape(Character & character, std::vector< std::string> & line)
{
	if (line.size() > 1)
	{
		if (line[0] == "a" || line[0] == "an")
		{
			switch(getShapeType(line))
			{
			case shNone:
				return failUnknownLine(line);
			case shLine:
				return parseLine(character,line);
			case shCircle:
				return parseCircle(character,line);
			case shSegment:
				return parseSegment(character,line);
			}
		}
	}
	return false;
}

bool ShapeParser::parseJoin(Character & character, std::vector< std::string> & line)
{
	if (contains(line, "joins"))
	{
		Join* join = parseJoinJoin(character, line);
		currentRule(character).joins.push_back(join);
		return true;
	}
	else if (contains(line, "intersects"))
	{
		Join* join = parseJoinJoin(character, line);

		setJoinToIntersection(*join, contains(line, "never"));

		currentRule(character).joins.push_back(join);

		return true;
	} 
	return false;
}

void ShapeParser::setJoinToIntersection(Join & join, bool parallel)
{
	join.first->type  = parallel ? ptNone : ptMiddle;
	join.second->type = parallel ? ptNone : ptMiddle;
}

//FIXME: memory leaks likely
Join* ShapeParser::parseJoinJoin(Character & character, TokenString & line)
{
	Join* join = NULL;
	std::vector< std::string> leftPart, rightPart;
	if (breakJoinString(line, leftPart, rightPart))
	{
		join = Join::create();
		join->first = getPart(character,leftPart);
		join->second = getPart(character,rightPart);
	}
	else
	{
		failUnknownLine(line);
	}
	return join;
}


Part* ShapeParser::getPart(Character & character, TokenString & line)
{
	int shapeNo = getShapeNo(line);
	if (shapeNo == -1)
		failUnknownLine(line);

	Part* result = new Part(currentRule(character).shapes[shapeNo], getPartType(line));
	return result;
}

PartType ShapeParser::getPartType(const TokenString & _line)
{
	TokenString line = _line;
	stripShapeNo(line);
	strip(line,"of");

	if (line.empty())
		return ptAny;
	if (contains(line,"top"))
		return ptTop;
	if (contains(line,"bottom"))
		return ptBottom;
	if (contains(line,"left"))
		return ptLeft;
	if (contains(line,"right"))
		return ptRight;
	if (contains(line,"middle"))
		return ptMiddle;

	if (contains(line,"1"))
		return pt1;
	if (contains(line,"2"))
		return pt2;
	if (contains(line,"3"))
		return pt3;
	if (contains(line,"4"))
		return pt4;
	if (contains(line,"5"))
		return pt5;
	if (contains(line,"6"))
		return pt6;
	if (contains(line,"7"))
		return pt7;
	if (contains(line,"8"))
		return pt8;
	if (contains(line,"9"))
		return pt9;
	if (contains(line,"10"))
		return pt10;
	if (contains(line,"11"))
		return pt11;
	if (contains(line,"12"))
		return pt12;

	failUnknownLine(_line);

	return ptNone;
}

bool ShapeParser::breakJoinString(const TokenString & line, TokenString & leftPart, TokenString & rightPart)
{
	TokenString::const_iterator joinPos = std::find(line.begin(), line.end(), "joins");

	if (joinPos == line.end())
		joinPos = std::find(line.begin(), line.end(), "intersects");

	if (joinPos == line.end())
		joinPos = std::find(line.begin(), line.end(), "is");

	if (joinPos == line.end())
		return failUnknownLine(line);

	TokenString::const_iterator secondStart = joinPos + 1;

	if (*(joinPos-1) == "never")
		--joinPos;

	if (*joinPos == "is")
	{
		secondStart += 2;
		if (*secondStart == "the")
			secondStart += 3;
	}

	std::copy(line.begin(), joinPos, std::back_inserter(leftPart));
	std::copy(secondStart, line.end(), std::back_inserter(rightPart));
	return true;
}

ShapeType ShapeParser::getShapeType(std::vector< std::string> & line)
{
	if(line.back() == "line")
		return shLine;
	if (line.back() == "circle")
		return shCircle;
	if (line[line.size() - 2] == "to")
		return shSegment;
	
	return shNone;
}

bool ShapeParser::parseLine(Character & character, TokenString & line)
{
	Shape* result = new Shape();
	result->type = shLine;
	result->direction = getDirection(line);
	result->size = getSize(line);
	currentRule(character).shapes.push_back(result);
	return true;
}

bool ShapeParser::parseCircle(Character & character, TokenString & line)
{
	Shape* result = new Shape();
	result->type = shCircle;
	result->direction = ddVertical;
	result->size = getSize(line);
	currentRule(character).shapes.push_back(result);
	return true;
}

bool ShapeParser::parseSegment(Character & character, TokenString & line)
{
	Shape* result = new Shape();
	result->type = shSegment;
	result->direction = ddVertical;
	result->size = getSize(line);
	result->start = getStart(line);
	result->end = getEnd(line);
	currentRule(character).shapes.push_back(result);
	return true;
}

Rule & ShapeParser::currentRule(Character & character)
{
	return CharacterRules::rules()[character].back();
}

Direction ShapeParser::getDirection(const TokenString & line)
{
	if (contains(line,"horizontal"))
		return ddHorizontal;
	if (contains(line,"vertical"))
		return ddVertical;
	if (contains(line,"slash"))
		return ddSlash;
	if (contains(line,"backslash"))
		return ddBackslash;

	return ddAny;
}

int ShapeParser::getShapeNo(TokenString & line)
{
	if (contains(line,"first"))
		return 0;
	if (contains(line,"second"))
		return 1;
	if (contains(line,"third"))
		return 2;
	if (contains(line,"fourth"))
		return 3;
	if (contains(line,"fifth"))
		return 4;
	if (contains(line,"sixth"))
		return 5;
	if (contains(line,"seventh"))
		return 6;
	if (contains(line,"eighth"))
		return 7;
	if (contains(line,"ninth"))
		return 8;
	if (contains(line,"tenth"))
		return 9;

	return -1;
}

void ShapeParser::stripShapeNo(TokenString & line)
{
	strip(line, "first");
	strip(line, "second");
	strip(line, "third");
	strip(line, "fourth");
	strip(line, "fifth");
	strip(line, "sixth");
	strip(line, "seventh");
	strip(line, "eighth");
	strip(line, "ninth");
	strip(line, "tenth");
}

bool ShapeParser::failUnknownLine(const TokenString & line)
{
	exit(13);
	return false;
}

ShapeSize ShapeParser::getSize(const TokenString & line)
{
	if (contains(line, "large") || contains(line, "long"))
	{
		return ssLarge;
	}
	if (contains(line, "short") || contains(line, "small")) //short and small are synonims
	{
		if (contains(line, "very"))
			return ssVeryShort;
		else
			return ssSmall;
	}
	return ssAny;
}

int ShapeParser::getStart(const TokenString & line)
{
	return intAroundTo(line,-1);
}

int ShapeParser::getEnd(const TokenString & line)
{
	return intAroundTo(line,1);
}

int ShapeParser::intAroundTo(const TokenString & line, int offset)
{
	TokenString::const_iterator it = std::find(line.begin(), line.end(), "to");

	if (it == line.end())
		return failUnknownLine(line);

	return atoi((*(it+offset)).c_str());
}

void ShapeParser::COMPATIBILITY_V1_fillDefaultNumericSequence()
{
	CharacterRules::numbers().push_back(Character("Zero"));
	CharacterRules::numbers().push_back(Character("One"));
	CharacterRules::numbers().push_back(Character("Two"));
	CharacterRules::numbers().push_back(Character("Three"));
	CharacterRules::numbers().push_back(Character("Four"));
	CharacterRules::numbers().push_back(Character("Five"));
	CharacterRules::numbers().push_back(Character("Six"));
	CharacterRules::numbers().push_back(Character("Seven"));
	CharacterRules::numbers().push_back(Character("Eight"));
	CharacterRules::numbers().push_back(Character("Nine"));
}