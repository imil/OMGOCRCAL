#include "StdAfx.h"

#include "Globals.h"
#include "ShapeReader.h"

ShapeReader::ShapeReader(void)
{
}

ShapeReader::~ShapeReader(void)
{
}

int ShapeReader::numfrom(const TokenString & string, int pos)
{
	return atoi(string[pos].c_str());
}

Point ShapeReader::pointfrom(const TokenString & tokens)
{
	return Point(numfrom(tokens,1), numfrom(tokens,2));
}

CompositeImage ShapeReader::read(const std::string & fileName)
{
	Box box;
	CompositeImage result;
	const std::vector<Point> emptyvec;

	AllTokens allTokens = tokenizeFile(fileName);

	for(AllTokens::iterator it = allTokens.begin(); it != allTokens.end(); ++it)
	{
		TokenString & tokens = *it;

		if (!tokens.empty())
		{
			std::string & key = tokens[0];
			if (key == "Box")
			{
				box = Box(Point(0,0), pointfrom(tokens));
			}
			else if (key == "Shape")
			{
				result.push_back(Image(box,emptyvec));
			}
			else if (key == "Point")
			{
				result.back().add(pointfrom(tokens));
			}
			else if (key == "epahS")
			{
			}
			else
			{
				Globals::log("Unknown token");
				Globals::log(key);
			}
		}
	}

	return result;
}
