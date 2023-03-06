#pragma once
#include <vector>
#include <regex>
#include <fstream>
#include "ParsedModule.h"

using namespace std;

string trim(string& str);
void is_clocked(ParsedModule* module);
void parse(ParsedModule* module, string file);
