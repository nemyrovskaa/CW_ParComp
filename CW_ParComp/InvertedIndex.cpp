#include "InvertedIndex.h"

InvertedIndex::InvertedIndex(string index_dir, unsigned int parser_th_num, unsigned int inverter_th_num)
{
    m_index_dir = index_dir;
    for (unsigned int i = 48; i < 58; i++)
        m_alphabet.push_back(i);

    for (unsigned int i = 97; i < 123; i++)
        m_alphabet.push_back(i);

    sort(m_alphabet.begin(), m_alphabet.end());

    m_tp = new ThreadPool();
    m_parser_th_num = parser_th_num;
    m_inverter_th_num = inverter_th_num;
    m_stop_pool = false;

    m_terms = new map<pair<char, char>, map<string, set<string>>>;
}

InvertedIndex::~InvertedIndex()
{
    for (auto task : m_tasks)
        delete(task);

    delete(m_terms);
    delete(m_tp);
}

vector<vector<string>> InvertedIndex::SplitDocs()
{
    vector<Split> splits;
    for (unsigned int i = 0; i < m_parser_th_num; i++) {
        Split split = { .split_size = 0, .file_paths = new vector<string>() };
        splits.push_back(split);
    }

    SplitDocs(splits, m_index_dir);

    vector<vector<string>> path_splits;
    for (auto i : splits)
    {
        path_splits.push_back(*i.file_paths);
        delete(i.file_paths);
    }

    return path_splits;
}

void InvertedIndex::SplitDocs(vector<Split>& splits, string dirPath)
{
    for (const auto& entry : filesystem::directory_iterator(dirPath)) {
        string file_path = entry.path().string();

        if (entry.is_directory())
        {
            SplitDocs(splits, dirPath + "\\" + entry.path().filename().string());
        }
        else
        {
            Split* min_size_split = &(*min_element(splits.begin(), splits.end()));
            min_size_split->split_size += entry.file_size();
            min_size_split->file_paths->push_back(file_path);
        }
    }
}

vector<pair<char, char>> InvertedIndex::SplitAlphabet()
{
    unsigned int split_size = m_alphabet.size() / m_inverter_th_num;
    vector<unsigned int> split_sizes(m_inverter_th_num, split_size);

    for (unsigned int i = 0; i < m_alphabet.size() % m_inverter_th_num; i++)
        split_sizes[i] += 1;

    vector<pair<char, char>> split_bounds;
    unsigned int start = 0;
    unsigned int end = start;
    for (auto i : split_sizes)
    {
        end = start + i - 1;
        split_bounds.push_back(make_pair(m_alphabet[start], m_alphabet[end]));
        start = end + 1;
    }

    return split_bounds;
}

map<pair<char, char>, vector<map<string, set<string>>>> InvertedIndex::SplitTerms()
{
    vector<pair<char, char>> split_bounds = SplitAlphabet();
    map<pair<char, char>, vector<map<string, set<string>>>> res_map;

    for (auto parser : m_tasks)
    {
        for (auto bound : split_bounds) {
            map<string, set<string>> temp_term_split;
            for (auto term : ((Parser*)parser)->getTerms()) {
                {
                    if ((term.first[0] >= bound.first) &&
                        (term.first[0] <= bound.second))
                        temp_term_split.insert(term);
                }
            }
            res_map[bound].push_back(temp_term_split);
        }
    }

    return res_map;
}

void InvertedIndex::EmptyPoolCallback()
{
    {
        mutex check_task_mutex;
        unique_lock<mutex> u_lock(check_task_mutex);
        m_stop_pool = true;
        for (auto i : m_tasks)
            if (i->getStatus() != Status::kExecuted &&
                i->getStatus() != Status::kIsProcessing)
                m_stop_pool = false;
    }
    m_stop_pool_var.notify_all();
}

void InvertedIndex::BuildIndex()
{
    vector<map<string, set<string>>> terms_vec;
    vector<vector<string>> docs_splits = SplitDocs();
    
    for (auto i : docs_splits)
    {
        Parser* parser = new Parser(i);
        m_tasks.push_back(parser);
    }

    m_tp->Init(bind(&InvertedIndex::EmptyPoolCallback, this), m_parser_th_num);
    for (auto parser : m_tasks)
        m_tp->AddTask(parser);

    {
        mutex m;
        unique_lock<mutex> u_lock(m);

        m_stop_pool_var.wait(u_lock, [this] {return m_stop_pool; });
    }
    m_tp->BreakSafe();
    


    map<pair<char, char>, vector<map<string, set<string>>>> splitedTerms = SplitTerms();
    m_tasks.clear();
    m_stop_pool = false;

    for (auto split : splitedTerms)
    {
        Inverter* inverter = new Inverter(split.second);
        m_tasks.push_back(inverter);
    }

    m_tp->Init(bind(&InvertedIndex::EmptyPoolCallback, this), m_inverter_th_num);
    for (auto inverter : m_tasks)
        m_tp->AddTask(inverter);

    {
        mutex m;
        unique_lock<mutex> u_lock(m);

        m_stop_pool_var.wait(u_lock, [this] {return m_stop_pool; });
    }
    m_tp->BreakSafe();


    for (auto inverter : m_tasks) {
        map<string, set<string>> merged_terms = ((Inverter*)inverter)->getTerms();
        char start = merged_terms.begin()->first[0];
        char end = (--merged_terms.end())->first[0];
        m_terms->insert(make_pair(make_pair(start, end), merged_terms));
    }
}

set<string> InvertedIndex::SearchDocs(string word)
{
    Parser* parser = new Parser();
    parser->NormalizeWord(word);

    set<string> ret_set;
    for (auto i : *m_terms)
    {
        if ((word[0] >= i.first.first) &&
            (word[0] <= i.first.second) &&
            i.second.contains(word))
            ret_set = i.second.at(word);
    }

    return ret_set;
}

map<pair<char, char>, map<string, set<string>>>& InvertedIndex::getTerms()
{
    return *m_terms;
}