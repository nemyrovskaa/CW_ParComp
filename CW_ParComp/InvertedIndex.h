#pragma once

#include <iostream>
#include <map>
#include <set>
#include <vector>

#include <functional>

#include "Parser.h"
#include "Inverter.h"
#include "ThreadPool.h"

using namespace std;

class InvertedIndex
{
private:
	string indexDir;
	vector<Task*> tasks;
	map<string, set<string>>* terms;
	ThreadPool* tp;
	int parserThreadNum;
	int inverterThreadNum;
	bool stopPool;
	condition_variable stopPoolVar;
	vector<char> alphabet;

	struct Split {

		uintmax_t splitSize;
		vector<string>* filePaths;

		inline bool operator< (const Split& other) { return splitSize < other.splitSize; }
	};
	vector<vector<string>> splitDocs();
	void splitDocs(vector<Split>&, string);
	vector<pair<char, char>> splitAlphabet();

public:
	InvertedIndex(string, int, int);
	~InvertedIndex();


	void buildIndex();
	set<string> searchDocs(string);
	map<string, set<string>>& getTerms();

	void emptyPoolCallback();
};