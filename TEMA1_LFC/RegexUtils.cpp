#include "RegexUtils.h"
#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <stack>

struct NFA {
    std::set<int> states;
    std::set<char> alphabet;
    std::map<std::pair<int, char>, std::set<int>> transitions;
    std::map<int, std::set<int>> lambda_transitions;
    int initial_state;
    int final_state;

    NFA() : initial_state(-1), final_state(-1) {}
};

bool isOperator(char c) {
    return (c == '|' || c == '.' || c == '*' || c == '(' || c == ')');
}

int priority(char op) {
    if (op == '|') return 1;
    if (op == '.') return 2;
    if (op == '*') return 3;
    return 0;
}

std::string addConcatenation(std::string regex) {
    std::string result = "";
    for (int i = 0; i < regex.length(); i++) {
        result += regex[i];

        if (i + 1 < regex.length()) {
            char current = regex[i];
            char next = regex[i + 1];

            if ((current != '(' && current != '|') &&
                (next != ')' && next != '|' && next != '*')) {
                result += '.';
            }
        }
    }
    return result;
}

std::string regexToPostfix(std::string regex) {
    regex = addConcatenation(regex);

    std::string postfix = "";
    std::stack<char> stack;

    for (int i = 0; i < regex.length(); i++) {
        char c = regex[i];

        if (!isOperator(c)) {
            postfix += c;
        }
        else if (c == '(') {
            stack.push(c);
        }
        else if (c == ')') {
            while (!stack.empty() && stack.top() != '(') {
                postfix += stack.top();
                stack.pop();
            }
            if (!stack.empty()) stack.pop();
        }
        else {
            while (!stack.empty() && stack.top() != '(' &&
                priority(stack.top()) >= priority(c)) {
                postfix += stack.top();
                stack.pop();
            }
            stack.push(c);
        }
    }

    while (!stack.empty()) {
        postfix += stack.top();
        stack.pop();
    }

    return postfix;
}

std::set<int> lambdaClosure(std::set<int> states, std::map<int, std::set<int> >& lambda_trans) {
    std::set<int> result = states;
    std::stack<int> to_process;
    for (std::set<int>::iterator it = states.begin(); it != states.end(); ++it) {
        to_process.push(*it);
    }
    while (!to_process.empty()) {
        int state = to_process.top();
        to_process.pop();

        if (lambda_trans.find(state) != lambda_trans.end()) {
            std::set<int>& next = lambda_trans[state];
            for (std::set<int>::iterator it = next.begin(); it != next.end(); ++it) {
                if (result.find(*it) == result.end()) {
                    result.insert(*it);
                    to_process.push(*it);
                }
            }
        }
    }

    return result;
}


DeterministicFiniteAutomaton RegexToDFA(std::string regex) {
    std::cout << "\n=== STEP 1: Convert to postfix notation ===" << std::endl;
    std::string postfix = regexToPostfix(regex);
    std::cout << "Original expression: " << regex << std::endl;
    std::cout << "Postfix form: " << postfix << std::endl;

    std::cout << "\n=== STEP 2: Build NFA with lambda transitions ===" << std::endl;

    std::stack<NFA> nfa_stack;
    int state_counter = 0;

    for (int idx = 0; idx < postfix.length(); idx++) {
        char c = postfix[idx];

        if (!isOperator(c)) {
            NFA new_nfa;
            new_nfa.initial_state = state_counter++;
            new_nfa.final_state = state_counter++;
            new_nfa.states.insert(new_nfa.initial_state);
            new_nfa.states.insert(new_nfa.final_state);
            new_nfa.alphabet.insert(c);

            std::pair<int, char> key(new_nfa.initial_state, c);
            new_nfa.transitions[key].insert(new_nfa.final_state);

            nfa_stack.push(new_nfa);
            std::cout << "  Character '" << c << "': q" << new_nfa.initial_state
                << " -> q" << new_nfa.final_state << std::endl;
        }
        else if (c == '.') {
            NFA B = nfa_stack.top(); nfa_stack.pop();
            NFA A = nfa_stack.top(); nfa_stack.pop();

            NFA C;
            C.initial_state = A.initial_state;
            C.final_state = B.final_state;

            C.states = A.states;
            C.states.insert(B.states.begin(), B.states.end());

            C.alphabet = A.alphabet;
            C.alphabet.insert(B.alphabet.begin(), B.alphabet.end());

            C.transitions = A.transitions;
            C.transitions.insert(B.transitions.begin(), B.transitions.end());

            C.lambda_transitions = A.lambda_transitions;
            C.lambda_transitions.insert(B.lambda_transitions.begin(), B.lambda_transitions.end());

            C.lambda_transitions[A.final_state].insert(B.initial_state);

            nfa_stack.push(C);
            std::cout << "  Concatenation: q" << C.initial_state << " -> q" << C.final_state << std::endl;
        }
        else if (c == '|') {
            NFA B = nfa_stack.top(); nfa_stack.pop();
            NFA A = nfa_stack.top(); nfa_stack.pop();

            NFA C;
            C.initial_state = state_counter++;
            C.final_state = state_counter++;

            C.states.insert(C.initial_state);
            C.states.insert(C.final_state);
            C.states.insert(A.states.begin(), A.states.end());
            C.states.insert(B.states.begin(), B.states.end());

            C.alphabet = A.alphabet;
            C.alphabet.insert(B.alphabet.begin(), B.alphabet.end());

            C.transitions = A.transitions;
            C.transitions.insert(B.transitions.begin(), B.transitions.end());

            C.lambda_transitions = A.lambda_transitions;
            C.lambda_transitions.insert(B.lambda_transitions.begin(), B.lambda_transitions.end());

            C.lambda_transitions[C.initial_state].insert(A.initial_state);
            C.lambda_transitions[C.initial_state].insert(B.initial_state);

            C.lambda_transitions[A.final_state].insert(C.final_state);
            C.lambda_transitions[B.final_state].insert(C.final_state);

            nfa_stack.push(C);
            std::cout << "  Alternation: q" << C.initial_state << " -> q" << C.final_state << std::endl;
        }
        else if (c == '*') {
            NFA A = nfa_stack.top(); nfa_stack.pop();

            NFA C;
            C.initial_state = state_counter++;
            C.final_state = state_counter++;

            C.states.insert(C.initial_state);
            C.states.insert(C.final_state);
            C.states.insert(A.states.begin(), A.states.end());

            C.alphabet = A.alphabet;
            C.transitions = A.transitions;
            C.lambda_transitions = A.lambda_transitions;

            C.lambda_transitions[C.initial_state].insert(A.initial_state);
            C.lambda_transitions[A.final_state].insert(C.final_state);
            C.lambda_transitions[C.initial_state].insert(C.final_state);
            C.lambda_transitions[A.final_state].insert(A.initial_state);

            nfa_stack.push(C);
            std::cout << "  Kleene star: q" << C.initial_state << " -> q" << C.final_state << std::endl;
        }
    }

    NFA final_nfa = nfa_stack.top();

    std::cout << "\n=== STEP 3: Convert NFA to DFA ===" << std::endl;

    DeterministicFiniteAutomaton dfa;

    std::map<std::set<int>, int> set_to_state;
    std::map<int, std::set<int> > state_to_set;
    int dfa_counter = 0;

    std::set<int> init_set;
    init_set.insert(final_nfa.initial_state);
    std::set<int> init_closure = lambdaClosure(init_set, final_nfa.lambda_transitions);

    dfa.q0 = dfa_counter++;
    set_to_state[init_closure] = dfa.q0;
    state_to_set[dfa.q0] = init_closure;
    dfa.Q.insert(dfa.q0);

    std::vector<std::set<int> > to_process;
    to_process.push_back(init_closure);

    dfa.Sigma = final_nfa.alphabet;

    int process_index = 0;
    while (process_index < to_process.size()) {
        std::set<int> current_states = to_process[process_index++]; 

        for (std::set<char>::iterator ch = dfa.Sigma.begin(); ch != dfa.Sigma.end(); ++ch) {
            char symbol = *ch;
            std::set<int> next;

            for (std::set<int>::iterator st = current_states.begin(); st != current_states.end(); ++st) {
                std::pair<int, char> key(*st, symbol);
                if (final_nfa.transitions.find(key) != final_nfa.transitions.end()) {
                    std::set<int>& destinations = final_nfa.transitions.at(key);
                    next.insert(destinations.begin(), destinations.end());
                }
            }

            if (!next.empty()) {
                next = lambdaClosure(next, final_nfa.lambda_transitions);

                if (set_to_state.find(next) == set_to_state.end()) {
                    int new_state = dfa_counter++;
                    set_to_state[next] = new_state;
                    state_to_set[new_state] = next;
                    dfa.Q.insert(new_state);
                    to_process.push_back(next);
                }

                std::pair<int, char> dfa_key(set_to_state[current_states], symbol);
                dfa.delta[dfa_key] = set_to_state[next];
            }
        }
    }

    for (std::map<int, std::set<int> >::iterator it = state_to_set.begin();
        it != state_to_set.end(); ++it) {
        if (it->second.find(final_nfa.final_state) != it->second.end()) {
            dfa.F.insert(it->first);
        }
    }

    return dfa;
}
