// CW_ParComp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Parser.h"
#include "Inverter.h"
#include "InvertedIndex.h"

int main()
{
    string str = "<h1>So <l>it's</l> a </h1>little<br /><br />dated now, > < it's almost 30 yrs old. Amazingly enough I have this on BETA tape and it still plays just fine. If it came to DVD I'd snap it up in a heartbeat.<br /><br />The drug humor is not appreciated nowadays as it was back then. Then it wasn't as 'harmful'. Much like driving without airbags, seat belts and child seats. I can remember my father crying he was laughing so hard watching this. I had coworkers in the 90's who'd seen it and I could bust them up by getting on the intercom and saying \"Iiiiiiiivvvvyyyyyyyyyyyyyyyy\".<br /><br />Great lines, great spoof of the original, and funny to me anyway even three decades later!";
    cout << str << endl << endl;

    InvertedIndex invertedIndex("I:\\university\\4-course-1-term\\course_work\\dataset_test", 2, 5);

    invertedIndex.buildIndex();
    map<string, set<string>> allTerms = invertedIndex.getTerms();

    /*for (auto i : allTerms)
    {
        cout << i.first << endl;
        for (auto j : i.second)
        {
            cout << j << endl;
        }
        cout << endl;
    }*/

    set<string> docs = invertedIndex.searchDocs("guide");

    for (auto i : docs)
        cout << i << endl;

}