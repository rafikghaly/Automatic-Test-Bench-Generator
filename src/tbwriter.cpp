#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "stimulusgen.h"
#include "parsedModule.h"
#include "CONFIG.h"

using namespace std;

void DUT(std::ofstream& ftb, const ParsedModule* module) {
	ftb << "// instantiate design instance" << endl;
	ftb << module->module_name << " DUT (" << endl;

	if (module->is_clocked) {
		ftb << "\t." << module->clock_name << '(' << module->clock_name << "),\n";
	}
	for (auto i : module->input_ports)
		ftb << "\t." << i.first << "(" << i.first << "_tb)," << endl;
	for (int i = 0; i < module->output_ports.size(); i++) {
		ftb << "\t." << module->output_ports[i].first << "(" << module->output_ports[i].first << "_tb)";
		if (i != module->output_ports.size() - 1)
			ftb << ',';
		ftb << endl;
	}

	ftb << ");" << endl << "endmodule" << endl;
}

void clk_geneneration(std::ofstream& ftb, const ParsedModule* module) {
	if (module->is_clocked == 1) {
		ftb << "//clock generation\n";
		ftb << "always #" << CLK_PERIOD / 2 << " " << module->clock_name << " = ! " << module->clock_name << ";\n\n";
	}
}


void tb_module_generator(const ParsedModule* module) {

	string outputfile = module->module_name + "_tb.v";
	std::ofstream ftb(outputfile);

	if (!ftb.is_open()) {
		cout << "file not openned!!" << endl;
	}
	ftb << "`timescale 1ns/1ps \n \n";
	ftb << "module " << module->module_name << "_tb ();";
	ftb << endl << endl;

	ftb << "//inputs\n";
	if (module->is_clocked == 1)
		ftb << "reg \t" << module->clock_name << ";\n";
	for (auto i : module->input_ports) ftb << "reg \t" << i.second << " " << i.first << "_tb;" << endl;
	ftb << "\n//outputs\n";
	for (auto i : module->output_ports) ftb << "wire \t" << i.second << " " << i.first << "_tb;" << endl;

	ftb << endl;

	clk_geneneration(ftb, module);
	do_it_random(ftb, module, ITERATIONS);
	ftb << endl << endl;

	DUT(ftb, module);
}

