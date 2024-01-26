#pragma once

#include <iostream>
#include <map>
#include <set>
#include <vector>

#include "Task.h"

using namespace std;

class Inverter : public Task
{
private:
	vector<map<string, set<string>>>* termsVec;
	map<string, set<string>>* terms;

public:

	Inverter();
	Inverter(vector<map<string, set<string>>>);
	~Inverter();

	void addTerms(map<string, set<string>>&);
	void reduseTerms();
	map<string, set<string>>& getTerms();

	void do_work() override
	{
		set_status(Status::IS_PROCESSING);
		reduseTerms();
		set_status(Status::EXECUTED);
	}
};