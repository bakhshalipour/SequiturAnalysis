#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <string>
#include <list>
#include <vector>
#include <sstream>

#define MAX_STREAM_LENGTH	(1 << 9)

using namespace std;

bool *head;
std::string **grammar;
std::map<int, int> countDict;
int *totalSymCount;
bool *visitedSymbols;

int calcTotalSymsCount(int line)
{
    if (visitedSymbols[line]) {
        return totalSymCount[line];
    }

    assert(countDict.find(line) != countDict.end());

    int total = 0;
    for (int i = 0; i < countDict[line]; i++) {
        string token = grammar[line][i];

        if (token[0] == '[') {
            total++;
        } else {
            total += calcTotalSymsCount(std::stoi(token));
        }
    }

    visitedSymbols[line] = true;
    totalSymCount[line] = total;

    return total;
}

int calcLen(std::string token) {
    if (token[0] == '[') { // Symbol
        return 1;
    }

    int index = std::stoi(token);
    if (head[index]) { // Visited
        return 1;
    }

    head[index] = true;
    int penalty = 0;
    int count = countDict[index];
    for (int i = 0; i < count; i++) {
        penalty += calcLen(grammar[index][i]);
    }

    return penalty;
}

int main(int argc, char const *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " path/to/grammar input_length" << std::endl;
        return 1;
    }

    // Read the grammar file
    ifstream grammarFile;
    grammarFile.open(argv[1], ios::in);
    std::list<std::string> lines;
    std::string tempStr;
    while (std::getline(grammarFile, tempStr)) {
        lines.push_back(tempStr);
    }

    // Parse rules
    const int headLength = lines.size();
    head = new bool[headLength]();
    std::string *rule = new std::string[headLength];

    int i = 0;
    while (!lines.empty()) {
        std::string line = lines.front();
        lines.pop_front();
        int pos = line.find('-');
        rule[i] = line.substr(pos + 3);
        i++;
    }

    // Parse grammar
    auto convertStrToWords = [](std::string str) {
        std::vector<std::string> outVec;
        std::stringstream ss(str);
        for (std::string t; ss >> t;) {
            outVec.push_back(t);
        }

        return outVec;
    };

    grammar = new std::string*[headLength];
    for (int i = 0; i < headLength; i++) {
        const int count = std::count(rule[i].begin(), rule[i].end(), ' ');
        countDict.insert(std::make_pair(i, count));
        grammar[i] = new std::string[count];
        auto words = convertStrToWords(std::string(rule[i]));
        for (int j = 0; j < count; j++) {
            grammar[i][j] = words[j];
        }
    }

    int penalty = calcLen("0");

    // Calculate Non-Repetitive Syms
    int nonRepetitiveSyms = 0;
    for (int i = 0; i < countDict[0]; i++) {
        std::string temp = grammar[0][i];
        if (temp[0] == '[') {
            nonRepetitiveSyms++;
        }
    }

    // Calculate New Syms
    int newSyms = 0;
    std::set<std::string> visitedSyms;
    for (int i = 1; i < headLength; i++) {
        for (int j = 0; j < countDict[i]; j++) {
            std::string currentSym = grammar[i][j];
            if (currentSym[0] != '[') continue; // None-terminal
            if (visitedSyms.find(currentSym) != visitedSyms.end()) continue;    // Already counted
            visitedSyms.insert(currentSym);
            newSyms++;
        }
    }

    int inputLength = atoi(argv[2]);
    std::cout << "Opportunity Analysis" << std::endl;
    std::cout << "Opportunity: " << 1.0 * (inputLength - penalty) / inputLength << std::endl;
    std::cout << "New: " << 1.0 * newSyms / inputLength << std::endl;
    std::cout << "Non-Repetitive: " << 1.0 * nonRepetitiveSyms / inputLength << std::endl;
    std::cout << "Head: " << 1.0 * (penalty - nonRepetitiveSyms - newSyms) / inputLength << std::endl; 

    totalSymCount = new int[headLength]();
    visitedSymbols = new bool [headLength]();
    for (int i = 1; i < headLength; i++) {
        totalSymCount[i] = calcTotalSymsCount(i);
    }

    int totalStreamsLength = 0;
    int totalStreamsCount = 0;
    int streamsStats[MAX_STREAM_LENGTH + 1] = {0};

    for (int i = 0; i < countDict[0]; i++) {
        string temp = grammar[0][i];
        if (temp[0] == '[') continue;

        int rule_id = std::stoi(temp);
        assert(rule_id != 0);

        int opportunity = totalSymCount[rule_id];
        assert(opportunity != 0);

        if (opportunity < MAX_STREAM_LENGTH) {
            streamsStats[opportunity]++;
        } else {
            streamsStats[MAX_STREAM_LENGTH]++;
        }

        totalStreamsLength += opportunity;
        totalStreamsCount++;
    }

    std::cout << "Average Stream Length = " << 1.0 * totalStreamsLength / totalStreamsCount << endl;
    int cummulativeStreamsStats[MAX_STREAM_LENGTH + 1] = {0};
    for (int i = 1; i <= MAX_STREAM_LENGTH; i++) {
        cummulativeStreamsStats[i] = cummulativeStreamsStats[i - 1] + streamsStats[i];
    }

    for (int i = 2; i <= MAX_STREAM_LENGTH; i *= 2) {
        std::cout << "Length<=" << i << ": " << 1.0 * cummulativeStreamsStats[i] / \
            cummulativeStreamsStats[MAX_STREAM_LENGTH] << endl;
    }

    return 0;
}
