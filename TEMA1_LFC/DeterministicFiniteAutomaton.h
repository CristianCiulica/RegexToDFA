#pragma once
#include <set>
#include <map>
#include <string>

class DeterministicFiniteAutomaton {
public:
    std::set<int> Q;
    std::set<char> Sigma;
    std::map<std::pair<int, char>, int> delta;
    int q0;
    std::set<int> F;
    DeterministicFiniteAutomaton();

    bool VerifyAutomaton();
    void PrintAutomaton();
    bool CheckWord(std::string word);
};