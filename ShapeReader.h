#pragma once

#include "Image.h"
#include "Parser.h"

//Reads image from file. Not really used anymore
class ShapeReader : public Parser
{
public:
	ShapeReader(void);
	~ShapeReader(void);

	CompositeImage read(const std::string & fileName);
private:
	Point pointfrom(const TokenString & string);
	int numfrom(const TokenString & string, int pos);
};
