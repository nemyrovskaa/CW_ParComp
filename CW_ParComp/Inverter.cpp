#include "Inverter.h"

Inverter::Inverter()
{
	m_terms = new map<string, set<string>>();
	m_terms_vec = new vector<map<string, set<string>>>();
}

Inverter::Inverter(vector<map<string, set<string>>> terms_vec)
{
	m_terms = new map<string, set<string>>();
	m_terms_vec = new vector<map<string, set<string>>>(terms_vec);
}

Inverter::~Inverter()
{
	delete(m_terms);
	delete(m_terms_vec);
}

void Inverter::AddTerms(map<string, set<string>>& inTerms)
{
	m_terms_vec->push_back(inTerms);
}

void Inverter::ReduseTerms()
{
	for (auto i : *m_terms_vec)
	{
		if (m_terms->empty())
			m_terms->insert(i.begin(), i.end());
		else
			for (auto term : i)
			{
				if (m_terms->contains(term.first))
					m_terms->find(term.first)->second.merge(term.second);
				else
					m_terms->insert(make_pair(term.first, term.second));
			}
	}
}

map<string, set<string>>& Inverter::getTerms()
{
	return *m_terms;
}


