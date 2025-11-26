#pragma once
#include <string>
#include <vector>
#include "DeterministicFiniteAutomaton.h"

struct SyntaxNode {
    std::string value;
    SyntaxNode* left;
    SyntaxNode* right;

    SyntaxNode(std::string v) : value(v), left(nullptr), right(nullptr) {}
};

std::string regexToPostfix(std::string regex);
DeterministicFiniteAutomaton RegexToDFA(std::string regex);
SyntaxNode* buildSyntaxTree(std::string postfix);
void printSyntaxTree(SyntaxNode* root);
void deleteTree(SyntaxNode* root);