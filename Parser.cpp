#include "StdAfx.h"
#include "Parser.h"

Parser::Parser(void)
{
}

Parser::~Parser(void)
{
}

const int MAX_LINE_LENGTH = 10000;

bool readline(std::ifstream & inputStream, std::string & line)
{
	char buf[MAX_LINE_LENGTH];
	inputStream.getline(buf, MAX_LINE_LENGTH);
	line = buf;
	return inputStream.rdstate() == 0;
}

std::string stripComment(std::string & line)
{
	size_t pos = line.find('\'');
	if (pos != line.npos)
	{
		if (pos == 0 || line[pos-1] != ' ')
			return "";
		else
			return line.substr(0, pos);
	}
	return line;
}

std::vector < std::vector< std::string> > Parser::tokenizeFile(const std::string & fileName)
{
	std::ifstream inputStream(fileName.c_str());

	std::vector < std::vector< std::string> > allTokens;

	std::string line;
	while(readline(inputStream, line))
	{
		line = stripComment(line);
		if (! line.empty())
		{
			std::vector<std::string> tokens = tokenize(line);
			allTokens.push_back(tokens);
			
		}
	}

	return allTokens;
}

std::vector<std::string> Parser::tokenize(std::string line)
{
	std::vector<std::string> result;
	std::istringstream inputStream(line);
	while(inputStream.rdstate() == 0)
	{
		std::string token;
		inputStream >> token;
		if (! token.empty() )
			result.push_back(token);
	}
	return result;
}

bool Parser::contains(const TokenString & line, const std::string & string) const
{
	return std::find(line.begin(), line.end(), string) != line.end();
}

void Parser::strip(TokenString & line, const std::string & string) const
{
	TokenString::iterator it = std::remove(line.begin(), line.end(), string);
	if (it != line.end())
	{
		line.erase(it);
	}
}
