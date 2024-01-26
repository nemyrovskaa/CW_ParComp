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
public:
	InvertedIndex(string, unsigned int, unsigned int);
	~InvertedIndex();

	void BuildIndex();
	set<string> SearchDocs(string);
	map<pair<char, char>, map<string, set<string>>>& getTerms();

private:
	struct Split {

		uintmax_t split_size;
		vector<string>* file_paths;

		inline bool operator< (const Split& other) { return split_size < other.split_size; }
	};

	vector<vector<string>> SplitDocs();
	void SplitDocs(vector<Split>&, string);
	vector<pair<char, char>> SplitAlphabet();
	map<pair<char, char>, vector<map<string, set<string>>>> SplitTerms();
	void EmptyPoolCallback();

	string m_index_dir;
	vector<char> m_alphabet;
	ThreadPool* m_tp;
	unsigned int m_parser_th_num;
	unsigned int m_inverter_th_num;
	bool m_stop_pool;
	condition_variable m_stop_pool_var;
	vector<Task*> m_tasks;
	map<pair<char, char>, map<string, set<string>>>* m_terms;
};