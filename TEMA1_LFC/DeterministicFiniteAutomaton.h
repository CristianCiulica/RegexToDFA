#pragma once
#include <set>
#include <map>
#include <string>
#include <vector>

class DeterministicFiniteAutomaton {
public:
    std::set<int> Q;
    std::set<char> Sigma;
    std::map<std::pair<int, char>, int> delta;
    int q0;
    std::set<int> F;

    DeterministicFiniteAutomaton();

    bool VerificaAutomat();
    void AfiseazaAutomat();
    bool VerificaCuvant(std::string cuvant);
};