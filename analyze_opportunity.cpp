#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <bits/stdc++.h> 

using namespace std;

bool *head;
string **grammar;
std::map<int, int> countDict;

int calcLen(string token)
{
    if (token[0] == '[') //Symbol
        return 1;

    int index = std::stoi(token);
    if (head [index])   //Visited
        return 1;

    head [index] = true;

    int penalty = 0;
    int count = countDict[index];
    for (int i = 0; i < count; i++)
        penalty += calcLen(grammar[index][i]);

    return penalty;
}

int main(int argc, char const *argv[])
{
    if (argc != 3) 
    {
        std::cerr << "Usage: " << argv[0] << " path/to/grammar input_length" << std::endl;
        return 1;
    }

    //Read the grammar file
    ifstream grammarFile;
    grammarFile.open(argv[1], ios::in);
    std::list<string> lines;
    string tempStr;
    while (std::getline(grammarFile, tempStr))
        lines.push_back(tempStr);
    //


    //Parse rules
    const int headLength = lines.size();
    head = new bool[headLength]();
    string *rule = new string[headLength];

    int i = 0;
    while (!lines.empty())
    {
        string line = lines.front();
        lines.pop_front();
        int pos = line.find('-');
        rule[i] = line.substr(pos + 3);
        i++;
    }
    //

    //Parse grammar
    grammar = new string*[headLength];
    for (int i = 0; i < headLength; i++)
    {
        const int count = std::count(rule[i].begin(), rule[i].end(), ' ');
        countDict.insert(make_pair(i, count));
        grammar[i] = new string[count];
        std::vector<string> words;
        boost::split(words, rule[i], boost::is_any_of(" "), boost::token_compress_on);
        for (int j = 0; j < count; j++)
            grammar[i][j] = words[j];
    }
    //


    int penalty = calcLen("0");

    //Calculate Non-Repetitive Syms
    int nonRepetitiveSyms = 0;
    for (int i = 0; i < countDict[0]; i++)
    {
        string temp = grammar[0][i];
        if (temp[0] == '[')
            nonRepetitiveSyms++;
    }
    //

    //Calculate New Syms
    int newSyms = 0;
    std::map<string, bool> visitedSyms;

    for (int i = 1; i < headLength; i++)
    {
        for (int j = 0; j < countDict[i]; j++)	
        {
            string currentSym = grammar[i][j];
            if (currentSym[0] != '[')
                continue;	//None-Terminal

            if (visitedSyms.find(currentSym) != visitedSyms.end())
                continue;	//Has already been counted
			
            visitedSyms.insert(make_pair(currentSym, true));
            newSyms++;
        }
    }
    //

    int inputLength = atoi(argv[2]);
    std::cout << "OPPORTUNITY ANALYSIS" << std::endl;
    std::cout << fixed << setprecision(2) << "Opportunity: " << (float)(inputLength - penalty) / inputLength * 100 << "%" << std::endl;
    std::cout << fixed << setprecision(2) << "New: " << (float) newSyms / inputLength * 100 << "%" << std::endl;
    std::cout << fixed << setprecision(2) << "Non-Repetitive: " << (float) nonRepetitiveSyms / inputLength * 100 << "%" << std::endl;
    std::cout << fixed << setprecision(2) << "Head: " << (float) (penalty - nonRepetitiveSyms - newSyms) / inputLength * 100 << "%" << std::endl; 

    return 0;
}
