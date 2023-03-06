#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include "parsedModule.h"

using namespace std;

int width_to_size(string s);
bool is_number(const string& s);
bool verilog_to_int(string& s);
void monitor(std::ofstream& ftb, const ParsedModule* module);
void initialize(std::ofstream& ftb, const ParsedModule* module);
void map_if_conditions(const ParsedModule* module, unordered_map<string, int>& mp);
void map_case_conditions(const ParsedModule* module, unordered_map<string, int>& mp_case);
void handle_if_conditions(std::ofstream& ftb, const ParsedModule* module, int index, pair<string, string> port);
void handle_case_conditions(std::ofstream& ftb, const ParsedModule* module, int index, pair<string, string> port);
void do_it_random(std::ofstream& ftb, const ParsedModule* module, int itr);
