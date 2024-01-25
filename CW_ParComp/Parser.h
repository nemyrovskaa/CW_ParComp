#pragma once

#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <string>
#include <set>
#include <map>

#include "Task.h"

using namespace std;

class Parser : public Task
{
private:
	vector<string> filePaths;
	set<string>* stopWords;
	map<string, set<string>>* terms;

	void parseText(string, string);

	void toLowerCase(string&);
	void removeTags(string&);
	void removeSpecialChars(string&);

public:
	Parser();
	Parser(vector<string>);
	~Parser();

	void normalizeWord(string&);
	void mapTerms();
	map<string, set<string>>& getTerms();
	void do_work() override
	{
		set_status(Status::IS_PROCESSING);
		mapTerms();
		set_status(Status::EXECUTED);
	}
};
