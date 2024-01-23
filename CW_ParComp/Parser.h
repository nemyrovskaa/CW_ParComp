#pragma once

#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <string>
#include <set>
#include <map>

using namespace std;

class Parser
{
private:
	string directoryPath;
	set<string> stopWords;

	void mapTerms(map<string, set<string>>&, string);
	void parseText(map<string, set<string>>&, string, string);

	void toLowerCase(string&);
	void removeTags(string&);
	void removeSpecialChars(string&);

public:
	Parser(string);
	~Parser();

	map<string, set<string>> mapTerms();
};
