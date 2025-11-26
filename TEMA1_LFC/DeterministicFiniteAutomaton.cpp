#include "DeterministicFiniteAutomaton.h"
#include <iostream>
#include <iomanip>
#include "Colors.h" 

DeterministicFiniteAutomaton::DeterministicFiniteAutomaton() {
    q0 = -1;
}

bool DeterministicFiniteAutomaton::VerificaAutomat() {
    if (Q.find(q0) == Q.end() && q0 != -1) {
        std::cout << "Eroare: Starea initiala nu este in Q!" << std::endl;
        return false;
    }
    for (int f : F) {
        if (Q.find(f) == Q.end()) {
            std::cout << "Eroare: Starea finala " << f << " nu este in Q!" << std::endl;
            return false;
        }
    }
    for (auto const& [key, dest] : delta) {
        int source = key.first;
        char symbol = key.second;

        if (Q.find(source) == Q.end()) {
            std::cout << "Eroare: Starea sursa " << source << " nu este in Q!" << std::endl;
            return false;
        }
        if (Q.find(dest) == Q.end()) {
            std::cout << "Eroare: Starea destinatie " << dest << " nu este in Q!" << std::endl;
            return false;
        }
        if (Sigma.find(symbol) == Sigma.end()) {
            std::cout << "Eroare: Simbolul '" << symbol << "' nu este in Sigma!" << std::endl;
            return false;
        }
    }
    return true;
}

void DeterministicFiniteAutomaton::AfiseazaAutomat() {
    std::cout << "\n" << BLUE << "---------- AUTOMAT FINIT DETERMINIST ----------" << RESET << std::endl;
    std::cout << "Stari (Q): { ";
    for (int s : Q) std::cout << "q" << s << " ";
    std::cout << "}" << std::endl;

    std::cout << "Alfabet (Sigma): { ";
    for (char c : Sigma) std::cout << c << " ";
    std::cout << "}" << std::endl;

    std::cout << "Stare initiala: q" << q0 << std::endl;

    std::cout << "Stari finale (F): { ";
    for (int s : F) std::cout << "q" << s << " ";
    std::cout << "}" << std::endl;

    std::cout << "\nTabela de tranzitii:" << std::endl;
    std::cout << BLUE << "---------------------" << RESET << std::endl;
    std::cout << "Stare\t| ";
    for (char c : Sigma) std::cout << c << "\t| ";
    std::cout << "\n" << BLUE << "---------------------" << RESET << std::endl;

    for (int s : Q) {
        std::cout << "q" << s << "\t| ";
        for (char c : Sigma) {
            if (delta.count({ s, c })) {
                std::cout << "q" << delta.at({ s, c }) << "\t| ";
            }
            else {
                std::cout << "-\t| ";
            }
        }
        std::cout << std::endl;
    }
    std::cout << BLUE << "---------------------" << RESET << std::endl;
}

bool DeterministicFiniteAutomaton::VerificaCuvant(std::string cuvant) {
    if (q0 == -1) {
        std::cout << RED << "Automat invalid (fara stare initiala)." << RESET << std::endl;
        return false;
    }

    int current_state = q0;
    std::cout << "\nVerificare cuvant: \"" << BOLD << cuvant << RESET << "\"" << std::endl;
    std::cout << "Tranzitii: q" << current_state;

    for (char c : cuvant) {
        if (Sigma.find(c) == Sigma.end()) {
            std::cout << " -> " << RED << "EROARE" << RESET << " (simbolul '" << c << "' nu e in alfabet)" << std::endl;
            std::cout << "Rezultat: " << RED << "RESPINS" << RESET << std::endl;
            return false;
        }

        if (delta.find({ current_state, c }) == delta.end()) {
            std::cout << " -> " << RED << "BLOCAJ" << RESET << " (nu exista tranzitie cu '" << c << "')" << std::endl;
            std::cout << "Rezultat: " << RED << "RESPINS" << RESET << std::endl;
            return false;
        }

        current_state = delta[{current_state, c}];
        std::cout << " -> q" << current_state;
    }

    std::cout << std::endl;
    bool accepted = (F.find(current_state) != F.end());
    if (accepted)
        std::cout << "Rezultat: " << GREEN << "ACCEPTAT" << RESET << std::endl;
    else
        std::cout << "Rezultat: " << RED << "RESPINS" << RESET << " (s-a terminat in starea q" << current_state << ", care nu e finala)" << std::endl;

    return accepted;
}