#include <iostream>
#include "DeterministicFiniteAutomaton.h"

DeterministicFiniteAutomaton::DeterministicFiniteAutomaton() {
    q0 = -1;
}

bool DeterministicFiniteAutomaton::VerifyAutomaton() {
    if (Q.find(q0) == Q.end() && q0 != -1) {
        std::cout << "Error: Initial state is not in Q!" << std::endl;
        return false;
    }
    for (std::set<int>::iterator it = F.begin(); it != F.end(); ++it) {
        if (Q.find(*it) == Q.end()) {
            std::cout << "Error: Final state " << *it << " is not in Q!" << std::endl;
            return false;
        }
    }
    for (std::map<std::pair<int, char>, int>::iterator it = delta.begin(); it != delta.end(); ++it) {
        int state = it->first.first;
        char symbol = it->first.second;
        int destination_state = it->second;

        if (Q.find(state) == Q.end()) {
            std::cout << "Error: State " << state << " from delta is not in Q!" << std::endl;
            return false;
        }
        if (Q.find(destination_state) == Q.end()) {
            std::cout << "Error: Destination state " << destination_state << " is not in Q!" << std::endl;
            return false;
        }
        if (Sigma.find(symbol) == Sigma.end()) {
            std::cout << "Error: Symbol " << symbol << " from delta is not in Sigma!" << std::endl;
            return false;
        }
    }

    return true;
}

void DeterministicFiniteAutomaton::PrintAutomaton() {
    std::cout << "\n========== DETERMINISTIC FINITE AUTOMATON ==========" << std::endl;
    std::cout << "States (Q): { ";
    for (std::set<int>::iterator it = Q.begin(); it != Q.end(); ++it) {
        std::cout << "q" << *it << " ";
    }
    std::cout << "}" << std::endl;

    std::cout << "Alphabet (Sigma): { ";
    for (std::set<char>::iterator it = Sigma.begin(); it != Sigma.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << "}" << std::endl;

    std::cout << "Initial state: q" << q0 << std::endl;

    std::cout << "Final states (F): { ";
    for (std::set<int>::iterator it = F.begin(); it != F.end(); ++it) {
        std::cout << "q" << *it << " ";
    }
    std::cout << "}" << std::endl;

    std::cout << "\nTransition table:" << std::endl;
    std::cout << "---------------------" << std::endl;
    std::cout << "State\t| ";
    for (std::set<char>::iterator it = Sigma.begin(); it != Sigma.end(); ++it) {
        std::cout << *it << "\t| ";
    }
    std::cout << std::endl;
    std::cout << "---------------------" << std::endl;

    for (std::set<int>::iterator it = Q.begin(); it != Q.end(); ++it) {
        std::cout << "q" << *it << "\t| ";
        for (std::set<char>::iterator ch = Sigma.begin(); ch != Sigma.end(); ++ch) {
            std::pair<int, char> key(*it, *ch);
            if (delta.find(key) != delta.end()) {
                std::cout << "q" << delta.at(key) << "\t| ";
            }
            else {
                std::cout << "-\t| ";
            }
        }
        std::cout << std::endl;
    }
    std::cout << "---------------------" << std::endl;
}

bool DeterministicFiniteAutomaton::CheckWord(std::string word) {
    if (q0 == -1) {
        std::cout << "Automaton has no initial state." << std::endl;
        return false;
    }
    int current_state = q0;

    std::cout << "\nChecking word: \"" << word << "\"" << std::endl;
    std::cout << "Transitions: q" << current_state;

    for (int i = 0; i < word.length(); i++) {
        char symbol = word[i];

        if (Sigma.find(symbol) == Sigma.end()) {
            std::cout << " -> BLOCKED (symbol '" << symbol << "' is not in alphabet)" << std::endl;
            std::cout << "Result: REJECTED" << std::endl;
            return false;
        }

        std::pair<int, char> key(current_state, symbol);

        if (delta.find(key) == delta.end()) {
            std::cout << " -> BLOCKED (no transition for '" << symbol << "')" << std::endl;
            std::cout << "Result: REJECTED" << std::endl;
            return false;
        }

        current_state = delta.at(key);
        std::cout << " -> q" << current_state;
    }

    std::cout << std::endl;
    bool accepted = (F.find(current_state) != F.end());
    std::cout << "Result: " << (accepted ? "ACCEPTED" : "REJECTED") << std::endl;
    return accepted;
}