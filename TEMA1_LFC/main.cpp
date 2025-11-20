#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <stack>
#include "DeterministicFiniteAutomaton.h"
#include "RegexUtils.h"

int main() {
    std::string regex;
    std::cout << "Regex: ";
    getline(std::cin, regex);

    DeterministicFiniteAutomaton dfa = RegexToDFA(regex);

    if (!dfa.VerificaAutomat()) {
        std::cout << "\nAutomatul nu este valid!" << std::endl;
        return 1;
    }

    std::cout << "\nDFA creat si verificat cu succes!" << std::endl;
    std::cout << "Numar de stari: " << dfa.Q.size() << std::endl;
    std::cout << "Stari finale: " << dfa.F.size() << std::endl;


    int option;
    do {
        std::cout << "\n========================================" << std::endl;
        std::cout << "MENIU:" << std::endl;
        std::cout << "1. Afiseaza notatia postfixata" << std::endl;
        std::cout << "2. Afiseaza automatul DFA" << std::endl;
        std::cout << "3. Salveaza automatul in fisier" << std::endl;
        std::cout << "4. Verifica un cuvant" << std::endl;
        std::cout << "0. Iesire" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "Optiune: ";

        while (!(std::cin >> option)) {
            std::cout << "Optiune invalida! Introduceti un numar: ";
            std::cin.clear();
            std::cin.ignore(1000, '\n');
        }
        std::cin.ignore(1000, '\n');

        switch (option) {
        case 1: {
            std::string postfix = regexToPostfix(regex);
            std::cout << "\nNotatia postfixata: " << postfix << std::endl;
            break;
        }
        case 2:
            dfa.AfiseazaAutomat();
            break;
        case 3: {
            std::ofstream fout("output.txt");
            if (fout.is_open()) {
                fout << "Expresie regulata: " << regex << std::endl;
                fout << "Forma postfixata: " << regexToPostfix(regex) << std::endl;

                fout << "\nAutomat Finit Determinist:" << std::endl;
                fout << "Stari (Q): { ";
                for (int state : dfa.Q) { fout << "q" << state << " "; }
                fout << "}" << std::endl;

                fout << "Alfabet (Sigma): { ";
                for (char symbol : dfa.Sigma) { fout << symbol << " "; }
                fout << "}" << std::endl;

                fout << "Stare initiala: q" << dfa.q0 << std::endl;

                fout << "Stari finale (F): { ";
                for (int state : dfa.F) { fout << "q" << state << " "; }
                fout << "}" << std::endl;

                fout << "\nTabela de tranzitii (delta):" << std::endl;
                for (auto const& [key, dest_state] : dfa.delta) {
                    fout << "  delta(q" << key.first << ", " << key.second << ") = q" << dest_state << std::endl;
                }

                fout.close();
                std::cout << "\nAutomatul a fost salvat in output.txt" << std::endl;
            }
            else {
                std::cout << "\nEroare la salvarea fisierului!" << std::endl;
            }
            break;
        }
        case 4: {
            std::string word;
            std::cout << "Introduceti cuvantul de verificat: ";
            getline(std::cin, word);
            dfa.VerificaCuvant(word);
            break;
        }
        case 0:
            std::cout << "\nLa revedere!" << std::endl;
            break;
        default:
            std::cout << "\nOptiune invalida!" << std::endl;
        }
    } while (option != 0);

    return 0;
}