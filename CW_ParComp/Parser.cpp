#include "Parser.h"

Parser::Parser()
{

}

Parser::Parser(vector<string> file_paths)
{
	m_file_paths = file_paths;
	m_stop_words = new set<string>();
	m_terms = new map<string, set<string>>();

	string file_path = "..\\resources\\stop_words.txt";

	ifstream file(file_path);
	if (!file)
		throw invalid_argument("Something went wrong while opening the file " + file_path);

	string word;
	while (getline(file, word))
		m_stop_words->insert(word);

	if (file.is_open())
		file.close();
}

Parser::~Parser()
{
	delete(m_terms);
	delete(m_stop_words);
}

void Parser::MapTerms()
{
	for (auto file_path : m_file_paths) {
		ifstream file(file_path);
		if (!file)
			throw invalid_argument("Something went wrong while opening the file " + file_path);

		stringstream file_ss;
		file_ss << file.rdbuf();
		string file_content = file_ss.str();

		ParseText(file_content, file_path);

		if (file.is_open())
			file.close();
	}
}

void Parser::NormalizeWord(string& str)
{
	ToLowerCase(str);
	RemoveSpecialChars(str);
}

void Parser::ParseText(string str, string file_path)
{
	RemoveTags(str);

	unsigned int start = 0;
	unsigned int end = 0;
	while (true)
	{
		while (str[start] == ' ')
			start++;
		end = str.find(" ", start);

		string token = str.substr(start, end - start);
		ToLowerCase(token);
		RemoveSpecialChars(token);
		if (token.size() != 0 && !m_stop_words->contains(token))
		{
			if (m_terms->contains(token))
				m_terms->find(token)->second.insert(file_path);
			else
			{
				set<string> docs;
				docs.insert(file_path);
				m_terms->insert(make_pair(token, docs));
			}
		}

		if (end == string::npos)
			break;

		start = end + 1;
	}
}

void Parser::RemoveTags(string& str)
{
	string open_tag;
	string close_tag;

	unsigned int search_from = 0;
	unsigned int open_brace_pos = 0;
	unsigned int close_brace_pos = 0;
	while (true)
	{
		open_brace_pos = str.find("<", search_from);
		close_brace_pos = str.find(">", open_brace_pos);

		if ((open_brace_pos == string::npos) || (close_brace_pos == string::npos))
		{
			break;
		}

		string tag_name = str.substr(open_brace_pos + 1, close_brace_pos - (open_brace_pos + 1));

		open_tag = "<" + tag_name + ">";
		close_tag = "</" + tag_name + ">";

		if (str.find(close_tag) == string::npos)
		{
			close_tag = open_tag;

			if (str.find(close_tag, str.find(open_tag) + open_tag.size()) == string::npos)
			{
				search_from = open_brace_pos + 1;
				continue;
			}
		}

		str.insert(str.find(open_tag), " ");
		str.erase(str.find(open_tag), open_tag.size());

		str.insert(str.find(close_tag), " ");
		str.erase(str.find(close_tag), close_tag.size());
	}
}

void Parser::ToLowerCase(string& str) {

	for (unsigned int i = 0; i < str.size(); i++) {
		str[i] = tolower(str[i]);
	}
}

void Parser::RemoveSpecialChars(string& str)
{
	unsigned int i = 0;
	while (i < str.size()) {
		if ((str[i] >= 48 && str[i] <= 57) ||
			(str[i] >= 65 && str[i] <= 90) ||
			(str[i] >= 97 && str[i] <= 122) ||
			(str[i] == 39))
		{
			i++;
		}
		else
		{
			str.erase(i, 1);
		}
	}

	i = 0;
	while (i < str.size()) {
		if (str[i] == 39 && (i == 0 || i == str.size() - 1))
			str.erase(i, 1);
		else
			i++;
	}
}

map<string, set<string>>& Parser::getTerms()
{
	return *m_terms;
}