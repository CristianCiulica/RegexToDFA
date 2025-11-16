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
    std::cout << "========================================" << std::endl;
    std::cout << "   REGEX TO DFA CONVERTER" << std::endl;
    std::cout << "========================================" << std::endl;

    std::string regex;
    std::ifstream fin("input.txt");
    if (fin.is_open()) {
        getline(fin, regex);
        fin.close();
        std::cout << "\nExpression read from file: " << regex << std::endl;
    }
    else {
        std::cout << "\nCould not open file input.txt" << std::endl;
        std::cout << "Enter regular expression: ";
        getline(std::cin, regex);
    }

    DeterministicFiniteAutomaton dfa = RegexToDFA(regex);

    if (!dfa.VerifyAutomaton()) {
        std::cout << "\nAutomaton is not valid!" << std::endl;
        return 1;
    }

    std::cout << "\nDFA created and verified successfully!" << std::endl;
    std::cout << "Number of states: " << dfa.Q.size() << std::endl;
    std::cout << "Final states: " << dfa.F.size() << std::endl;


    int option;
    do {
        std::cout << "\n========================================" << std::endl;
        std::cout << "MENU:" << std::endl;
        std::cout << "1. Display postfix notation" << std::endl;
        std::cout << "2. Display DFA automaton" << std::endl;
        std::cout << "3. Save automaton to file" << std::endl;
        std::cout << "4. Check a word" << std::endl;
        std::cout << "0. Exit" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "Option: ";

        while (!(std::cin >> option)) {
            std::cout << "Invalid option! Enter a number: ";
            std::cin.clear();
            std::cin.ignore(1000, '\n');
        }
        std::cin.ignore(1000, '\n');

        switch (option) {
        case 1: {
            std::string postfix = regexToPostfix(regex);
            std::cout << "\nPostfix notation: " << postfix << std::endl;
            break;
        }
        case 2:
            dfa.PrintAutomaton();
            break;
        case 3: {
            std::ofstream fout("output.txt");
            if (fout.is_open()) {
                fout << "Regular expression: " << regex << std::endl;
                fout << "Postfix form: " << regexToPostfix(regex) << std::endl;

                fout << "\nDeterministic Finite Automaton:" << std::endl;
                fout << "States (Q): { ";
                for (int state : dfa.Q) { fout << "q" << state << " "; }
                fout << "}" << std::endl;

                fout << "Alphabet (Sigma): { ";
                for (char symbol : dfa.Sigma) { fout << symbol << " "; }
                fout << "}" << std::endl;

                fout << "Initial state: q" << dfa.q0 << std::endl;

                fout << "Final states (F): { ";
                for (int state : dfa.F) { fout << "q" << state << " "; }
                fout << "}" << std::endl;

                fout << "\nTransition table (delta):" << std::endl;
                for (auto const& [key, dest_state] : dfa.delta) {
                    fout << "  delta(q" << key.first << ", " << key.second << ") = q" << dest_state << std::endl;
                }

                fout.close();
                std::cout << "\nAutomaton has been saved to output.txt" << std::endl;
            }
            else {
                std::cout << "\nError saving file!" << std::endl;
            }
            break;
        }
        case 4: {
            std::string word;
            std::cout << "Enter word to check: ";
            getline(std::cin, word);
            dfa.CheckWord(word);
            break;
        }
        case 0:
            std::cout << "\nGoodbye!" << std::endl;
            break;
        default:
            std::cout << "\nInvalid option!" << std::endl;
        }
    } while (option != 0);

    return 0;
}