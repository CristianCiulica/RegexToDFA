#pragma once
#include <string>
#include "DeterministicFiniteAutomaton.h"

std::string regexToPostfix(std::string regex);

DeterministicFiniteAutomaton RegexToDFA(std::string regex);
