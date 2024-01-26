#pragma once

#include <iostream>
#include <map>
#include <set>
#include <vector>

#include "Task.h"

using namespace std;

class Inverter : public Task
{
public:
	Inverter();
	Inverter(vector<map<string, set<string>>>);
	~Inverter();

	void AddTerms(map<string, set<string>>&);
	void ReduseTerms();
	map<string, set<string>>& getTerms();

	void DoWork() override
	{
		setStatus(Status::kIsProcessing);
		ReduseTerms();
		setStatus(Status::kExecuted);
	}

private:
	vector<map<string, set<string>>>* m_terms_vec;
	map<string, set<string>>* m_terms;
};