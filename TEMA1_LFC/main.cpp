#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h> 
#include "DeterministicFiniteAutomaton.h"
#include "RegexUtils.h"
#include "Colors.h" 

int main() {
    system("color");

    std::string regex;
    std::ifstream fin("input.txt");
    bool needConsoleInput = true;
    if (fin.is_open()) {
        if (fin.peek() == EOF) {
            std::cout << YELLOW << "[ATENTIE] " << RESET << "Fisierul input.txt exista, dar este " << RED << "GOL" << RESET << "." << std::endl;
        }
        else {
            std::getline(fin, regex);
            if (regex.empty()) {
                std::cout << YELLOW << "[ATENTIE] " << RESET << "Expresia citita din fisier este " << RED << "VIDA" << RESET << "." << std::endl;
            }
            else {
                std::cout << BLUE << "[INFO] " << RESET << "S-a citit din input.txt expresia: " << BOLD << regex << RESET << std::endl;
                needConsoleInput = false;
            }
        }
        fin.close();
    }
    else {
        std::cout << YELLOW << "[INFO] " << RESET << "Nu s-a gasit fisierul input.txt." << std::endl;
    }

    if (needConsoleInput) {
        std::cout << YELLOW << "[INPUT] " << RESET << "Introduceti regex manual: ";
        std::cout << YELLOW;
        std::getline(std::cin, regex);
        std::cout << RESET;
    }

    if (regex.empty()) {
        std::cout << RED << "[EROARE] Expresie vida." << RESET << std::endl;
        return 1;
    }
    DeterministicFiniteAutomaton dfa = RegexToDFA(regex);
    std::string postfix = regexToPostfix(regex);
    SyntaxNode* root = buildSyntaxTree(postfix);

    int option;
    do {
        std::cout << "\n" << BLUE << "--------------------------------" << RESET << std::endl;
        std::cout << "          " << CYAN << "MENIU " << RESET << std::endl;
        std::cout << BLUE << "--------------------------------" << RESET << std::endl;

        std::cout << "1. Afiseaza notatia postfixata" << std::endl;
        std::cout << "2. Afiseaza Arborele Sintactic" << std::endl;
        std::cout << "3. Afiseaza Automatul (AFD)" << std::endl;
        std::cout << "4. Salveaza Automatul in fisier" << std::endl;
        std::cout << "5. Verifica cuvinte" << std::endl;
        std::cout << "0. Iesire" << std::endl;

        std::cout << BLUE << "--------------------------------" << RESET << std::endl;
        std::cout << YELLOW << "Optiune: " << RESET;

        while (!(std::cin >> option)) {
            std::cout << RED << "Optiune invalida! Introduceti un numar: " << RESET;
            std::cin.clear();
            std::cin.ignore(1000, '\n');
        }
        std::cin.ignore(1000, '\n');

        switch (option) {
        case 1:
            std::cout << "\nNotatia postfixata: " << BOLD << postfix << RESET << std::endl;
            break;
        case 2:
            printSyntaxTree(root);
            break;
        case 3:
            dfa.AfiseazaAutomat();
            break;
        case 4: {
            std::ofstream fout("output.txt");
            if (fout.is_open()) {
                fout << "Expresie regulata: " << regex << "\n";
                fout << "Forma postfixata: " << postfix << "\n\n";

                fout << "--- AUTOMAT FINIT DETERMINIST ---\n";
                fout << "Stari (Q): { ";
                for (int s : dfa.Q) fout << "q" << s << " ";
                fout << "}\n";

                fout << "Alfabet (Sigma): { ";
                for (char c : dfa.Sigma) fout << c << " ";
                fout << "}\n";

                fout << "Stare initiala: q" << dfa.q0 << "\n";

                fout << "Stari finale (F): { ";
                for (int s : dfa.F) fout << "q" << s << " ";
                fout << "}\n";

                fout << "\nTranzitii:\n";
                for (auto const& [key, val] : dfa.delta) {
                    fout << "  delta(q" << key.first << ", " << key.second << ") = q" << val << "\n";
                }
                fout.close();
                std::cout << "\n" << GREEN << "[SUCCES] Automatul a fost salvat in output.txt" << RESET << std::endl;
            }
            else {
                std::cout << "\n" << RED << "[EROARE] Nu s-a putut crea fisierul" << RESET << std::endl;
            }
            break;
        }
        case 5: {
            std::string word;
            std::cout << "\n" << CYAN << "---Scrie \"stop\" pentru a reveni la meniu---" << RESET << std::endl;

            while (true) {
                std::cout << YELLOW << ">> Introdu cuvantul: " << RESET;
                std::getline(std::cin, word);

                if (word == "stop") {
                    std::cout << MAGENTA << "Revenire la meniu..." << RESET << std::endl;
                    break;
                }

                if (word == "_")
                    word = "";

                dfa.VerificaCuvant(word);
                std::cout << std::endl;
            }
            break;
        }
        case 0:
            std::cout << RED << "Program incheiat." << RESET << std::endl;
            break;
        default:
            std::cout << RED << "Optiune invalida." << RESET << std::endl;
        }
    } while (option != 0);

    deleteTree(root);
    return 0;
}