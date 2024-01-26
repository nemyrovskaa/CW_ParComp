// CW_ParComp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Parser.h"
#include "Inverter.h"
#include "InvertedIndex.h"

int main()
{
    cout << "\t\tInverted Index" << endl;

    string index_dir; // I:\\university\\4-course-1-term\\course_work\\dataset
    cout << "\nPlease, enter the path to directory:" << endl;
    cin >> index_dir;

    unsigned int parser_th_num = 0, inverter_th_num = 0;
    cout << "\nEnter amount of parser-threads: ";
    cin >> parser_th_num;
    cout << "Enter amount of inverter-threads: ";
    cin >> inverter_th_num;

    char answer;
    cout << "\nDo you want to start indexing process? (Y/N): ";
    cin >> answer;

    if (tolower(answer) != 'y')
        return 0;

    InvertedIndex invertedIndex(index_dir, parser_th_num, inverter_th_num);
    chrono::high_resolution_clock::time_point start_time;
    chrono::high_resolution_clock::time_point end_time;
    start_time = chrono::high_resolution_clock::now();
    invertedIndex.BuildIndex();
    end_time = chrono::high_resolution_clock::now();
    
    string word; //"guide";
    cout << "\nIndex was built successfully!" << endl;
    cout << "Enter the word you are searching: ";
    cin >> word;

    set<string> docs = invertedIndex.SearchDocs(word);
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

    unsigned long long time = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();
    cout << "\nIndexing time: " << time << " ms" << endl;
}