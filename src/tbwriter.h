#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "stimulusgen.h"
#include "ParsedModule.h"
#include "CONFIG.h"

using namespace std;

void DUT(std::ofstream& ftb, const ParsedModule* module);
void clk_geneneration(std::ofstream& ftb, const ParsedModule* module);
void tb_module_generator(const ParsedModule* module);
