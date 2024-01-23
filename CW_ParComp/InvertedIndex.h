#pragma once

#include <iostream>
#include <map>
#include <set>
#include <vector>

#include "Parser.h"
#include "Inverter.h"

using namespace std;

class InvertedIndex
{
private:
	string indexDir;
	map<string, set<string>>* terms;

public:
	InvertedIndex(string indexDir);
	~InvertedIndex();

	void buildIndex();
	set<string> searchDocs(string);
	map<string, set<string>>& getTerms();
};