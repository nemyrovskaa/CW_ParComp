#pragma once

#include <iostream>
#include <map>
#include <set>
#include <vector>

using namespace std;

class Inverter
{
private:
	vector<map<string, set<string>>>* termsVec;
	map<string, set<string>>* terms;

public:

	Inverter();
	Inverter(vector<map<string, set<string>>>*);
	~Inverter();

	void addTerms(map<string, set<string>>&);
	void reduseTerms();

	map<string, set<string>>& getTerms();
};