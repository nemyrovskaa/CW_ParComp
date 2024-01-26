#include "InvertedIndex.h"

InvertedIndex::InvertedIndex(string indexDir, int parserThreadNum, int inverterThreadNum)
{
	this->indexDir = indexDir;
    this->terms = new map<pair<char, char>, map<string, set<string>>>;
    this->tp = new ThreadPool();
    this->parserThreadNum = parserThreadNum;
    this->inverterThreadNum = inverterThreadNum;
    this->stopPool = false;

    for (size_t i = 48; i < 58; i++)
        alphabet.push_back(i);

    for (size_t i = 97; i < 123; i++)
        alphabet.push_back(i);

    sort(alphabet.begin(), alphabet.end());
}

InvertedIndex::~InvertedIndex()
{
    for (auto task : tasks)
        delete(task);

    delete(terms);
    delete(tp);
}

vector<vector<string>> InvertedIndex::splitDocs()
{
    vector<Split> splits;
    for (size_t i = 0; i < parserThreadNum; i++) {
        Split split = { .splitSize = 0, .filePaths = new vector<string>() };
        splits.push_back(split);
    }

    splitDocs(splits, indexDir);

    vector<vector<string>> pathSplits;
    for (auto i : splits)
    {
        pathSplits.push_back(*i.filePaths);
        delete(i.filePaths);
    }

    return pathSplits;
}

void InvertedIndex::splitDocs(vector<Split>& splits, string dirPath)
{
    for (const auto& entry : filesystem::directory_iterator(dirPath)) {
        string filePath = entry.path().string();

        if (entry.is_directory())
        {
            splitDocs(splits, dirPath + "\\" + entry.path().filename().string());
        }
        else
        {
            Split* minSizeSplit = &(*min_element(splits.begin(), splits.end()));
            minSizeSplit->splitSize += entry.file_size();
            minSizeSplit->filePaths->push_back(filePath);
        }
    }
}

vector<pair<char, char>> InvertedIndex::splitAlphabet()
{
    int splitSize = alphabet.size() / inverterThreadNum;
    vector<int> splitSizes(inverterThreadNum, splitSize);

    for (size_t i = 0; i < alphabet.size() % inverterThreadNum; i++)
        splitSizes[i] += 1;

    vector<pair<char, char>> splitBounds;
    int start = 0;
    int end = start;
    for (auto i : splitSizes)
    {
        end = start + i - 1;
        splitBounds.push_back(make_pair(alphabet[start], alphabet[end]));
        start = end + 1;
    }

    return splitBounds;
}

map<pair<char, char>, vector<map<string, set<string>>>> InvertedIndex::splitTerms()
{
    vector<pair<char, char>> splitBounds = splitAlphabet();
    map<pair<char, char>, vector<map<string, set<string>>>> resMap;

    for (auto parser : tasks)
    {
        for (auto bound : splitBounds) {
            map<string, set<string>> tempTermSplit;
            for (auto term : ((Parser*)parser)->getTerms()) {
                {
                    if ((term.first[0] >= bound.first) &&
                        (term.first[0] <= bound.second))
                        tempTermSplit.insert(term);
                }
            }
            resMap[bound].push_back(tempTermSplit);
        }
    }

    return resMap;
}

void InvertedIndex::emptyPoolCallback()
{
    {
        mutex checkTaskMutex;
        unique_lock<mutex> u_lock(checkTaskMutex);
        stopPool = true;
        for (auto i : tasks)
            if (i->get_status() != Status::EXECUTED &&
                i->get_status() != Status::IS_PROCESSING)
                stopPool = false;
    }
    stopPoolVar.notify_all();
}

void InvertedIndex::buildIndex()
{
    vector<map<string, set<string>>> termsVec;
    vector<vector<string>> docsSplits = splitDocs();
    
    for (auto i : docsSplits)
    {
        Parser* parser = new Parser(i);
        tasks.push_back(parser);
    }

    tp->init(bind(&InvertedIndex::emptyPoolCallback, this), parserThreadNum);
    for (auto parser : tasks)
        tp->add_task(parser);

    {
        mutex m;
        unique_lock<mutex> u_lock(m);

        stopPoolVar.wait(u_lock, [this] {return stopPool; });
    }
    tp->break_safe();
    


    map<pair<char, char>, vector<map<string, set<string>>>> splitedTerms = splitTerms();
    tasks.clear();
    stopPool = false;

    for (auto split : splitedTerms)
    {
        Inverter* inverter = new Inverter(split.second);
        tasks.push_back(inverter);
    }

    tp->init(bind(&InvertedIndex::emptyPoolCallback, this), inverterThreadNum);
    for (auto inverter : tasks)
        tp->add_task(inverter);

    {
        mutex m;
        unique_lock<mutex> u_lock(m);

        stopPoolVar.wait(u_lock, [this] {return stopPool; });
    }
    tp->break_safe();


    for (auto inverter : tasks) {
        map<string, set<string>> mergedTerms = ((Inverter*)inverter)->getTerms();
        char start = mergedTerms.begin()->first[0];
        char end = (--mergedTerms.end())->first[0];
        terms->insert(make_pair(make_pair(start, end), mergedTerms));
    }
}

set<string> InvertedIndex::searchDocs(string word)
{
    Parser* parser = new Parser();
    parser->normalizeWord(word);

    
    set<string> retSet;
    for (auto i : *terms)
    {
        if ((word[0] >= i.first.first) &&
            (word[0] <= i.first.second) &&
            i.second.contains(word))
            retSet = i.second.at(word);
    }

    return retSet;
}

map<pair<char, char>, map<string, set<string>>>& InvertedIndex::getTerms()
{
    return *this->terms;
}