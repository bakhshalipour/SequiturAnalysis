#include <assert.h>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <bits/stdc++.h> 

#define MAX_STREAM_LENGTH	(1 << 9)

using namespace std;

string **grammar;
std::map<int, int> countDict;
int *totalSymCount;
bool *visitedSymbols;

int calcTotalSymsCount(int line)
{
    if (visitedSymbols[line])
        return totalSymCount[line];

    assert(countDict.find(line) != countDict.end());

    int total = 0;

    for (int i = 0; i < countDict[line]; i++)
    {
        string token = grammar[line][i];

        if (token[0] == '[')
            total++;
        else
            total += calcTotalSymsCount(std::stoi(token));
    }

    visitedSymbols[line] = true;
    totalSymCount[line] = total;

    return total;
}

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " path/to/grammar" << std::endl;
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
    string *rule = new string[headLength];

    int i = 0;
    while (lines.size() != 0)
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
        std::vector<std::string> words;
        boost::split(words, rule[i], boost::is_any_of(" "), boost::token_compress_on);
        for (int j = 0; j < count; j++)
            grammar[i][j] = words[j];
    }
    //

    totalSymCount = new int[headLength]();
    visitedSymbols = new bool [headLength]();

    for (int i = 1; i < headLength; i++)
    {
        int total_syms = calcTotalSymsCount(i);
        totalSymCount[i] = total_syms;
    }

    int totalStreamsLength = 0;
    int totalStreamsCount = 0;
    int streamsStats[MAX_STREAM_LENGTH + 1] = {0};

    for (int i = 0; i < countDict[0]; i++)
    {
        string temp = grammar[0][i];
        if (temp[0] == '[')
            continue;

        int rule_id = std::stoi(temp);
        assert(rule_id != 0);

        int opportunity = totalSymCount[rule_id];
        assert(opportunity != 0);

        if (opportunity < MAX_STREAM_LENGTH)
            streamsStats[opportunity]++;
        else
            streamsStats[MAX_STREAM_LENGTH]++;

        totalStreamsLength += opportunity;
        totalStreamsCount++;
    }

    std::cout << fixed << setprecision(2) << "Average Stream Length = " << (float)totalStreamsLength / totalStreamsCount << endl;

    int cummulativeStreamsStats[MAX_STREAM_LENGTH + 1] = {0};

    for (int i = 1; i <= MAX_STREAM_LENGTH; i++)
        cummulativeStreamsStats[i] = cummulativeStreamsStats[i - 1] + streamsStats[i];

    for (int i = 2; i <= MAX_STREAM_LENGTH; i *= 2)
        std::cout << fixed << setprecision(2) << "Stream Length <= " << i << "\t Percent = " << (float)cummulativeStreamsStats[i] / cummulativeStreamsStats[MAX_STREAM_LENGTH] * 100 << "%" << endl;

    return 0;
}
