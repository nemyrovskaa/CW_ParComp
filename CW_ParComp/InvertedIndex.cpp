#include "InvertedIndex.h"

InvertedIndex::InvertedIndex(string indexDir, int parserThreadNum, int inverterThreadNum)
{
	this->indexDir = indexDir;
    this->terms = new map<string, set<string>>;
    this->tp = new ThreadPool();
    this->parserThreadNum = parserThreadNum;
    this->inverterThreadNum = inverterThreadNum;
    this->stopPool = false;

    for (int i = 48; i < 58; i++)
        alphabet.push_back(i);

    for (int i = 65; i < 91; i++)
        alphabet.push_back(i);

    for (int i = 97; i < 123; i++)
        alphabet.push_back(i);

    sort(alphabet.begin(), alphabet.end());
}

InvertedIndex::~InvertedIndex()
{

}

vector<vector<string>> InvertedIndex::splitDocs()
{
    vector<Split> splits;
    for (int i = 0; i < parserThreadNum; i++) {
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

    for (int i = 0; i < alphabet.size() % inverterThreadNum; i++)
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
    


    for (auto parser : tasks)
        termsVec.push_back(((Parser*)parser)->getTerms());

    Inverter* inverter = new Inverter(&termsVec);
    inverter->reduseTerms();

    *terms = inverter->getTerms();
}

set<string> InvertedIndex::searchDocs(string word)
{
    Parser* parser = new Parser();
    parser->normalizeWord(word);

    set<string>* s = new set<string>();
    return terms->contains(word) ? terms->at(word) : *s;
}

map<string, set<string>>& InvertedIndex::getTerms()
{
    return *this->terms;
}