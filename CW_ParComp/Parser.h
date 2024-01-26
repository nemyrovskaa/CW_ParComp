#pragma once

#include <iostream>
#include <fstream>
#include <filesystem>
#include <set>
#include <map>

#include "Task.h"

using namespace std;

class Parser : public Task
{
public:
	Parser();
	Parser(vector<string>);
	~Parser();

	void NormalizeWord(string&);
	void MapTerms();
	map<string, set<string>>& getTerms();

	void DoWork() override
	{
		setStatus(Status::kIsProcessing);
		MapTerms();
		setStatus(Status::kExecuted);
	}

private:
	void ParseText(string, string);
	void ToLowerCase(string&);
	void RemoveTags(string&);
	void RemoveSpecialChars(string&);

	vector<string> m_file_paths;
	set<string>* m_stop_words;
	map<string, set<string>>* m_terms;
};
