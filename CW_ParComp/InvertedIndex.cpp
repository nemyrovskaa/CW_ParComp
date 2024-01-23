#include "InvertedIndex.h"

InvertedIndex::InvertedIndex(string indexDir)
{
	this->indexDir = indexDir;
    this->terms = new map<string, set<string>>;
}

InvertedIndex::~InvertedIndex()
{

}

void InvertedIndex::buildIndex()
{
    vector<map<string, set<string>>> termsVec;

    Parser* parser = new Parser(indexDir);
    termsVec.push_back(parser->mapTerms());

    Inverter* inverter = new Inverter(&termsVec);
    inverter->reduseTerms();

    *terms = inverter->getTerms();
}

set<string> InvertedIndex::searchDocs(string word)
{
    Parser* parser = new Parser();
    parser->normalizeWord(word);


    return terms->at(word);
}

map<string, set<string>>& InvertedIndex::getTerms()
{
    return *this->terms;
}