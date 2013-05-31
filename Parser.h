#pragma once

//Base class for parsers: splits lines into tokens and removes comments
class Parser
{
protected:
	Parser(void);
	virtual ~Parser(void);

	typedef std::vector< std::string> TokenString;
	typedef std::vector<TokenString> AllTokens;

	AllTokens tokenizeFile(const std::string & fileName);
	TokenString tokenize(std::string line);

	bool contains(const TokenString & line, const std::string & string) const;
	void strip(TokenString & line, const std::string & string) const;
};
