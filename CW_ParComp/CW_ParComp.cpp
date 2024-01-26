// CW_ParComp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Parser.h"
#include "Inverter.h"
#include "InvertedIndex.h"

int main()
{
    cout << "\t\tInverted Index" << endl;

    string indexDir; // I:\\university\\4-course-1-term\\course_work\\dataset
    cout << "\nPlease, enter the path to directory:" << endl;
    cin >> indexDir;

    unsigned int parserThNum = 0, inverterThNum = 0;
    cout << "\nEnter amount of parser-threads: ";
    cin >> parserThNum;
    cout << "Enter amount of inverter-threads: ";
    cin >> inverterThNum;

    char buildIndex;
    cout << "\nDo you want to start indexing process? (Y/N): ";
    cin >> buildIndex;

    if (tolower(buildIndex) != 'y')
        return 0;

    InvertedIndex invertedIndex(indexDir, parserThNum, inverterThNum);
    invertedIndex.buildIndex();
    
    string word; //"guide";
    cout << "\nIndex was built successfully!" << endl;
    cout << "Enter the word you are searching: ";
    cin >> word;

    set<string> docs = invertedIndex.searchDocs(word);
    if (docs.empty())
    {
        cout << "\nIt's a pity, your word \"" << word << "\" wasn't found." << endl;
    }
    else
    {
        cout << "\nThe word \"" << word << "\" was found! You can see it in:" << endl;
        for (auto i : docs)
            cout << i << endl;
    }
}