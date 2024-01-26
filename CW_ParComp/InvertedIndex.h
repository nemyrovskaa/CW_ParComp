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
	vector<char> alphabet;

	ThreadPool* tp;
	int parserThreadNum;
	int inverterThreadNum;
	bool stopPool;
	condition_variable stopPoolVar;

	vector<Task*> tasks;
	map<pair<char, char>, map<string, set<string>>>* terms;

	struct Split {

		uintmax_t splitSize;
		vector<string>* filePaths;

		inline bool operator< (const Split& other) { return splitSize < other.splitSize; }
	};

	vector<vector<string>> splitDocs();
	void splitDocs(vector<Split>&, string);

	vector<pair<char, char>> splitAlphabet();
	map<pair<char, char>, vector<map<string, set<string>>>> splitTerms();

	void emptyPoolCallback();

public:
	InvertedIndex(string, int, int);
	~InvertedIndex();

	void buildIndex();
	set<string> searchDocs(string);
	map<pair<char, char>, map<string, set<string>>>& getTerms();
};