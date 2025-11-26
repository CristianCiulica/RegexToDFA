#include "RegexUtils.h"
#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <stack>
#include <algorithm>
#include <string>

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
    return (c == '|' || c == '.' || c == '*' || c == '+' || c == '(' || c == ')');
}

int priority(char op) {
    if (op == '|') return 1;
    if (op == '.') return 2;
    if (op == '*' || op == '+') return 3;
    return 0;
}

std::string addConcatenation(std::string regex) {
    std::string result = "";
    for (int i = 0; i < regex.length(); i++) {
        char c1 = regex[i];
        result += c1;

        if (i + 1 < regex.length()) {
            char c2 = regex[i + 1];

            bool c1_is_operand = !isOperator(c1);
            bool c2_is_operand = !isOperator(c2);

            bool c1_allows_dot_after = (c1_is_operand || c1 == '*' || c1 == '+' || c1 == ')');
            bool c2_allows_dot_before = (c2_is_operand || c2 == '(');

            if (c1_allows_dot_after && c2_allows_dot_before) {
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

    for (char c : regex) {
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
            while (!stack.empty() && stack.top() != '('
                && priority(stack.top()) >= priority(c)) {
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

std::set<int> lambdaClosure(std::set<int> states, std::map<int, std::set<int>>& lambda_trans) {
    std::set<int> result = states;
    std::stack<int> to_process;
    for (int s : states) to_process.push(s);

    while (!to_process.empty()) {
        int state = to_process.top();
        to_process.pop();

        if (lambda_trans.count(state)) {
            for (int next_state : lambda_trans[state]) {
                if (result.find(next_state) == result.end()) {
                    result.insert(next_state);
                    to_process.push(next_state);
                }
            }
        }
    }
    return result;
}

DeterministicFiniteAutomaton RegexToDFA(std::string regex) {
    std::string postfix = regexToPostfix(regex);
    std::stack<NFA> nfa_stack;
    int state_counter = 0;

    for (char c : postfix) {
        if (!isOperator(c)) {
            NFA nfa;
            nfa.initial_state = state_counter++;
            nfa.final_state = state_counter++;
            nfa.states = { nfa.initial_state, nfa.final_state };
            nfa.alphabet.insert(c);
            nfa.transitions[{nfa.initial_state, c}].insert(nfa.final_state);
            nfa_stack.push(nfa);
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
        }
        else if (c == '|') {
            NFA B = nfa_stack.top(); nfa_stack.pop();
            NFA A = nfa_stack.top(); nfa_stack.pop();
            NFA C;
            C.initial_state = state_counter++;
            C.final_state = state_counter++;

            C.states = A.states;
            C.states.insert(B.states.begin(), B.states.end());
            C.states.insert(C.initial_state);
            C.states.insert(C.final_state);
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
        }
        else if (c == '*' || c == '+') {
            NFA A = nfa_stack.top(); nfa_stack.pop();
            NFA C;
            C.initial_state = state_counter++;
            C.final_state = state_counter++;

            C.states = A.states;
            C.states.insert(C.initial_state);
            C.states.insert(C.final_state);
            C.alphabet = A.alphabet;
            C.transitions = A.transitions;
            C.lambda_transitions = A.lambda_transitions;

            C.lambda_transitions[C.initial_state].insert(A.initial_state);
            C.lambda_transitions[A.final_state].insert(C.final_state);
            C.lambda_transitions[A.final_state].insert(A.initial_state);

            if (c == '*') {
                C.lambda_transitions[C.initial_state].insert(C.final_state);
            }

            nfa_stack.push(C);
        }
    }

    NFA final_nfa = nfa_stack.top();

    DeterministicFiniteAutomaton dfa;
    int dfa_state_counter = 0;

    std::map<std::set<int>, int> set_to_id;
    std::vector<std::set<int>> process_queue;

    std::set<int> start_closure = lambdaClosure({ final_nfa.initial_state }, final_nfa.lambda_transitions);

    dfa.q0 = dfa_state_counter;
    set_to_id[start_closure] = dfa_state_counter++;
    dfa.Q.insert(dfa.q0);
    process_queue.push_back(start_closure);

    dfa.Sigma = final_nfa.alphabet;

    int idx = 0;
    while (idx < process_queue.size()) {
        std::set<int> current_set = process_queue[idx++];
        int current_id = set_to_id[current_set];

        if (current_set.count(final_nfa.final_state)) {
            dfa.F.insert(current_id);
        }

        for (char symbol : dfa.Sigma) {
            std::set<int> next_set;
            for (int s : current_set) {
                if (final_nfa.transitions.count({ s, symbol })) {
                    for (int dest : final_nfa.transitions[{s, symbol}]) {
                        next_set.insert(dest);
                    }
                }
            }

            if (!next_set.empty()) {
                std::set<int> closure = lambdaClosure(next_set, final_nfa.lambda_transitions);

                if (set_to_id.find(closure) == set_to_id.end()) {
                    set_to_id[closure] = dfa_state_counter;
                    dfa.Q.insert(dfa_state_counter);
                    process_queue.push_back(closure);
                    dfa_state_counter++;
                }

                dfa.delta[{current_id, symbol}] = set_to_id[closure];
            }
        }
    }

    return dfa;
}

SyntaxNode* buildSyntaxTree(std::string postfix) {
    std::stack<SyntaxNode*> st;

    for (char c : postfix) {
        if (!isOperator(c)) {
            st.push(new SyntaxNode(std::string(1, c)));
        }
        else {
            std::string displayValue = std::string(1, c);
            if (c == '|') displayValue = "(sau)";
            if (c == '.') displayValue = "(.)"; 
            if (c == '*') displayValue = "(*)"; 
            if (c == '+') displayValue = "(+)";    

            SyntaxNode* node = new SyntaxNode(displayValue);

            if (c == '*' || c == '+') {
                if (!st.empty()) {
                    node->left = st.top();
                    st.pop();
                }
            }
            else {
                if (!st.empty()) {
                    node->right = st.top();
                    st.pop();
                }
                if (!st.empty()) {
                    node->left = st.top();
                    st.pop();
                }
            }
            st.push(node);
        }
    }
    return st.empty() ? nullptr : st.top();
}

void printTreeRecursive(SyntaxNode* node, std::string prefix, bool isLast) {
    if (!node) return;

    std::cout << prefix;
    std::cout << (isLast ? "`-- " : "|-- ");
    std::cout << node->value << std::endl;

    std::string childPrefix = prefix + (isLast ? "    " : "|   ");

    if (node->left && node->right) {
        printTreeRecursive(node->left, childPrefix, false);
        printTreeRecursive(node->right, childPrefix, true);
    }
    else if (node->left) {
        printTreeRecursive(node->left, childPrefix, true);
    }
    else if (node->right) {
        printTreeRecursive(node->right, childPrefix, true);
    }
}

void printSyntaxTree(SyntaxNode* root) {
    if (!root) return;
    std::cout << "\nArbore Sintactic (Structura):\n";
    printTreeRecursive(root, "", true);
    std::cout << std::endl;
}

void deleteTree(SyntaxNode* root) {
    if (!root) return;
    deleteTree(root->left);
    deleteTree(root->right);
    delete root;
}