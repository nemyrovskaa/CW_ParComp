#include "Inverter.h"

Inverter::Inverter()
{
	this->terms = new map<string, set<string>>();
	this->termsVec = new vector<map<string, set<string>>>();
}

Inverter::Inverter(vector<map<string, set<string>>>* termsVec)
{
	this->terms = new map<string, set<string>>();
	this->termsVec = termsVec;
}

Inverter::~Inverter()
{
	delete(terms);
	delete(termsVec);
}

void Inverter::addTerms(map<string, set<string>>& inTerms)
{
	termsVec->push_back(inTerms);
}

void Inverter::reduseTerms()
{
	for (auto i : *termsVec)
	{
		if (terms->empty())
			terms->insert(i.begin(), i.end());
		else
			for (auto term : i)
			{
				if (terms->contains(term.first))
					terms->find(term.first)->second.merge(term.second);
				else
					terms->insert(make_pair(term.first, term.second));
			}
	}
}

map<string, set<string>>& Inverter::getTerms()
{
	return *this->terms;
}
