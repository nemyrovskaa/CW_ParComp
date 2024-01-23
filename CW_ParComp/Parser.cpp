#include "Parser.h"

Parser::Parser(string directoryPath)
{
	this->directoryPath = directoryPath;

	string filePath = "I:\\university\\4-course-1-term\\course_work\\stop_words.txt";

	ifstream file(filePath);
	if (!file)
		throw invalid_argument("Something went wrong while opening the file " + filePath);

	string word;
	while (getline(file, word))
		stopWords.insert(word);

	if (file.is_open())
		file.close();
}

Parser::~Parser()
{

}

map<string, set<string>> Parser::mapTerms()
{
	map<string, set<string>> tokens;

	mapTerms(tokens, this->directoryPath);

	return tokens;
}

void Parser::mapTerms(map<string, set<string>>& tokens, string directoryPath)
{
	for (const auto& entry : filesystem::directory_iterator(directoryPath)) {
		string filePath = entry.path().string();

		if (entry.is_directory())
		{
			mapTerms(tokens, directoryPath + "\\" + entry.path().filename().string());
		}
		else
		{
			ifstream file(filePath);
			if (!file)
				throw invalid_argument("Something went wrong while opening the file " + filePath);

			stringstream fileSS;
			fileSS << file.rdbuf();
			string fileContent = fileSS.str();

			parseText(tokens, fileContent, filePath);

			if (file.is_open())
				file.close();
		}
	}
}

void Parser::parseText(map<string, set<string>>& terms, string str, string filePath)
{
	removeTags(str);

	int start = 0;
	int end = 0;
	while (true)
	{
		while (str[start] == ' ')
			start++;
		end = str.find(" ", start);

		string token = str.substr(start, end - start);
		toLowerCase(token);
		removeSpecialChars(token);
		if (token.size() != 0 && !stopWords.contains(token))
		{
			if (terms.contains(token))
				terms.find(token)->second.insert(filePath);
			else
			{
				set<string> docs;
				docs.insert(filePath);
				terms.insert(make_pair(token, docs));
			}
		}

		if (end == string::npos)
			break;

		start = end + 1;
	}
}

void Parser::removeTags(string& str)
{
	string openTag;
	string closeTag;

	int searchFrom = 0;
	int openBracePos = 0;
	int closeBracePos = 0;
	while (true)
	{
		openBracePos = str.find("<", searchFrom);
		closeBracePos = str.find(">", openBracePos);

		if ((openBracePos == string::npos) || (closeBracePos == string::npos))
		{
			break;
		}

		string tagName = str.substr(openBracePos + 1, closeBracePos - (openBracePos + 1));

		openTag = "<" + tagName + ">";
		closeTag = "</" + tagName + ">";

		if (str.find(closeTag) == string::npos)
		{
			closeTag = openTag;

			if (str.find(closeTag, str.find(openTag) + openTag.size()) == string::npos)
			{
				searchFrom = openBracePos + 1;
				continue;
			}
		}

		str.insert(str.find(openTag), " ");
		str.erase(str.find(openTag), openTag.size());

		str.insert(str.find(closeTag), " ");
		str.erase(str.find(closeTag), closeTag.size());
	}
}

void Parser::toLowerCase(string& str) {

	for (int i = 0; i < str.size(); i++) {
		str[i] = tolower(str[i]);
	}
}

void Parser::removeSpecialChars(string& str)	//--------------------------------- TODO
{
	int i = 0;
	while (i < str.size()) {
		if ((str[i] >= 33 && str[i] <= 38) ||
			(str[i] >= 40 && str[i] <= 47) ||
			(str[i] >= 58 && str[i] <= 64) ||
			(str[i] >= 91 && str[i] <= 96) ||
			(str[i] >= 123 && str[i] <= 126))
		{
			str.erase(i, 1);
		}
		else
		{
			i++;
		}
	}

	i = 0;
	while (i < str.size()) {
		if (str[i] == 39 && (i == 0 || i == str.size() - 1))
		{
			str.erase(i, 1);
		}
		else
		{
			i++;
		}

	}
}