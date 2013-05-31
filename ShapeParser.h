#pragma once

#include "Shape.h"
#include "Part.h"
#include "CharacterRules.h"
#include "Parser.h"

//This class populates CharacterRules with rules from specified file
class ShapeParser : public Parser
{
public:
	ShapeParser(void);
	~ShapeParser(void);

	//Main method. Does all the work
	void parse(const std::string & fileName);

private:
	void parse(AllTokens & tokens);

	//These methods return false if unable to parse.
	//This allows to chain them using the || operator
	bool parseChardef(Character & character, TokenString & line);
	bool parseAlterRule(Character & character, TokenString & line);
	bool parseShape(Character & character, TokenString & line);
	bool parseJoin(Character & character, TokenString & line);
	bool parseLine(Character & character, TokenString & line);
	bool parseCircle(Character & character, TokenString & line);
	bool parseSegment(Character & character, TokenString & line);
	bool parseNumericSequenceDefinition(Character & character, TokenString & line);
	bool parseNumericSequence(Character & character, TokenString & line);

	//Parse join definition without regard to type
	Join* parseJoinJoin(Character & character, TokenString & line);
	//Applies type to join
	void setJoinToIntersection(Join & join, bool parallel);

	//Die horribly on unknown strings
	bool failUnknownLine(const TokenString & line);

	//Break join string into three parts
	bool breakJoinString(const TokenString & line, TokenString & leftPart, TokenString & rightPart);

	ShapeType getShapeType(TokenString & line);

	Rule & currentRule(Character & character);
	Part* getPart(Character & character, TokenString & line);
	int getShapeNo(TokenString & line);
	void stripShapeNo(TokenString & line);
	PartType getPartType(const TokenString & line);

	Direction getDirection(const TokenString & line);
	ShapeSize getSize(const TokenString & line);
	int getStart(const TokenString & line);
	int getEnd(const TokenString & line);
	int intAroundTo(const TokenString & line, int offset);

	//Return reserved character meaning that numeric sequence is being parsed
	Character numSeqTag();

	//Yes, we've got compatibility issues!
	//Quite a few users may have created their own shape definitions by this time
	//A pity that Raymond Chen saw only v1, he should appreciate this
	void COMPATIBILITY_V1_fillDefaultNumericSequence();
};
