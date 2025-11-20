#include <iostream>
#include "DeterministicFiniteAutomaton.h"

DeterministicFiniteAutomaton::DeterministicFiniteAutomaton() {
    q0 = -1;
}

bool DeterministicFiniteAutomaton::VerificaAutomat() {
    if (Q.find(q0) == Q.end() && q0 != -1) {
        std::cout << "Eroare: Starea initiala nu este in Q!" << std::endl;
        return false;
    }
    for (std::set<int>::iterator it = F.begin(); it != F.end(); ++it) {
        if (Q.find(*it) == Q.end()) {
            std::cout << "Eroare: Starea finala " << *it << " nu este in Q!" << std::endl;
            return false;
        }
    }
    for (std::map<std::pair<int, char>, int>::iterator it = delta.begin(); it != delta.end(); ++it) {
        int state = it->first.first;
        char symbol = it->first.second;
        int destination_state = it->second;

        if (Q.find(state) == Q.end()) {
            std::cout << "Eroare: Starea " << state << " din delta nu este in Q!" << std::endl;
            return false;
        }
        if (Q.find(destination_state) == Q.end()) {
            std::cout << "Eroare: Starea destinatie " << destination_state << " nu este in Q!" << std::endl;
            return false;
        }
        if (Sigma.find(symbol) == Sigma.end()) {
            std::cout << "Eroare: Simbolul " << symbol << " din delta nu este in Sigma!" << std::endl;
            return false;
        }
    }

    return true;
}

void DeterministicFiniteAutomaton::AfiseazaAutomat() {
    std::cout << "\n========== AUTOMAT FINIT DETERMINIST ==========" << std::endl;
    std::cout << "Stari (Q): { ";
    for (std::set<int>::iterator it = Q.begin(); it != Q.end(); ++it) {
        std::cout << "q" << *it << " ";
    }
    std::cout << "}" << std::endl;

    std::cout << "Alfabet (Sigma): { ";
    for (std::set<char>::iterator it = Sigma.begin(); it != Sigma.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << "}" << std::endl;

    std::cout << "Stare initiala: q" << q0 << std::endl;

    std::cout << "Stari finale (F): { ";
    for (std::set<int>::iterator it = F.begin(); it != F.end(); ++it) {
        std::cout << "q" << *it << " ";
    }
    std::cout << "}" << std::endl;

    std::cout << "\nTabela de tranzitii:" << std::endl;
    std::cout << "---------------------" << std::endl;
    std::cout << "Stare\t| ";
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

bool DeterministicFiniteAutomaton::VerificaCuvant(std::string cuvant) {
    if (q0 == -1) {
        std::cout << "Automatul nu are stare initiala." << std::endl;
        return false;
    }
    int current_state = q0;

    std::cout << "\nVerificare cuvant: \"" << cuvant << "\"" << std::endl;
    std::cout << "Tranzitii: q" << current_state;

    for (int i = 0; i < cuvant.length(); i++) {
        char symbol = cuvant[i];

        if (Sigma.find(symbol) == Sigma.end()) {
            std::cout << " -> BLOCAT (simbolul '" << symbol << "' nu este in alfabet)" << std::endl;
            std::cout << "Rezultat: RESPINS" << std::endl;
            return false;
        }

        std::pair<int, char> key(current_state, symbol);

        if (delta.find(key) == delta.end()) {
            std::cout << " -> BLOCAT (nu exista tranzitie pentru '" << symbol << "')" << std::endl;
            std::cout << "Rezultat: RESPINS" << std::endl;
            return false;
        }

        current_state = delta.at(key);
        std::cout << " -> q" << current_state;
    }

    std::cout << std::endl;
    bool accepted = (F.find(current_state) != F.end());
    std::cout << "Rezultat: " << (accepted ? "ACCEPTAT" : "RESPINS") << std::endl;
    return accepted;
}