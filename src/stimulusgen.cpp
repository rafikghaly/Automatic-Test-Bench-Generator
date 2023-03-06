#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include "parsedModule.h"

#define make_condition(x,y) to_string((int)((x*1.0/y)*100))

using namespace std;

int width_to_size(string s) {
    if (s == "") return 1;
    s = s.substr(1, s.size() - 2);
    int dot, a, b;
    dot = s.find(":");
    a = stoi(s.substr(0, dot));
    b = stoi(s.substr(dot + 1));
    return abs(a - b) + 1;
}

bool is_number(const string& s) {
    return s[0] > '0' && s[0] <= '9';
}

int verilog_to_int(string &v) {

    if (v.find("'") == string::npos) {
        return stoi(v);
    }
    else {

        int dot = v.find("'");
        int size = stoi(v.substr(0, dot));
        string sval = v.substr(dot + 2, v.size());
        int value = 0;
        switch (v[dot + 1]) {
        case 'b':
            value = std::strtol(sval.c_str(), nullptr, 2);
            break;
        case 'd':
            value = std::strtol(sval.c_str(), nullptr, 10);
            break;
        case 'o':
            value = std::strtol(sval.c_str(), nullptr, 8);
            break;
        case 'h':
            value = std::strtol(sval.c_str(), nullptr, 16);
            break;
        }
        return (value) & ((1 << size) - 1);
    }
}

void monitor(std::ofstream& ftb, const ParsedModule* module) {

    ftb << "/////////////////////////////////////////////////\n";
    ftb << "\t//monitor inputs\n";
    for (int i = 0; i < module->input_ports.size(); i++)
        ftb << "\t$monitor(" << '\"' << module->input_ports[i].first << " = %d\", " << module->input_ports[i].first << "_tb);\n";
    ftb << "\t//monitor outputs\n";
    for (int i = 0; i < module->output_ports.size(); i++)
        ftb << "\t$monitor(" << '\"' << module->output_ports[i].first << " = %d\", " << module->output_ports[i].first << "_tb);\n";
    ftb << "/////////////////////////////////////////////////\n";

}

void initialize(std::ofstream& ftb, const ParsedModule* module) {

    ftb << "\t// Initialize all Inputs by zero\n";

    if (module->is_clocked)
        ftb << "\t" << module->clock_name << " = 0;\n";

    for (int i = 0; i < module->input_ports.size(); i++)
        ftb << "\t" << module->input_ports[i].first << "_tb" << " = 0;\n";

    ftb << "\n\n";
}

void map_if_conditions(const ParsedModule* module, unordered_map<string, int>& mp_if) {

    for (int i = 0; i < module->input_ports.size(); i++)
        mp_if[module->input_ports[i].first] = -1;

    for (int i = 0; i < module->if_statements.size(); i++)
        mp_if[module->if_statements[i].identifier] = i;

}

void map_case_conditions(const ParsedModule* module, unordered_map<string, int>& mp_case) {

    for (int i = 0; i < module->input_ports.size(); i++)
        mp_case[module->input_ports[i].first] = -1;

    for (int i = 0; i < module->case_statements.size(); i++)
        mp_case[module->case_statements[i].identifier] = i;


}

void handle_case_conditions(std::ofstream& ftb, const ParsedModule* module, int index, pair<string, string> port) {
    string  port_name = port.first,
        port_width = port.second;
    int conditions = module->case_statements[index].condition.size(), num_conditions = 0;

    ftb << "\t\ttorandom = {$random} % 100;\n";

    for (auto& e : module->case_statements[index].condition) {
        if (is_number(e)) {
            ftb << "\t\tif(torandom >= " << make_condition(num_conditions++, conditions);
            ftb << " && torandom < " << make_condition(num_conditions, conditions) << ")\n";
            ftb << "\t\t\t" << port_name << "_tb" << " = " << e << ";\n";
        }
    }
    ftb << "\t\tif(torandom >= " << make_condition(num_conditions++, conditions) << ")\n";
    ftb << "\t\t\t" << port_name << "_tb" << " = {$random} % " << to_string(1 << width_to_size(port_width)) << ";\n";
}

void handle_if_conditions(std::ofstream& ftb, const ParsedModule* module, int index, pair<string, string> port) {
    string condition = module->if_statements[index].condition,
        value = module->if_statements[index].value,
        port_name = port.first,
        port_width = port.second;

    if (is_number(value)) {
        int v = verilog_to_int(value);
        if (condition == "==") {
            ftb << "\t\t" << port_name << "_tb" << " = " << v << ";\n";
            ftb << "\t\ttorandom = {$random} % 10;\n";
            ftb << "\t\tif(torandom > 4)\n";
            ftb << "\t\t\t" << port_name << "_tb" << " = {$random} % " << to_string(1 << width_to_size(port_width)) << ";\n";
        }
        else {
            ftb << "\t\t" << port_name << "_tb" << " = {$random} % " << v << ";\n";
            ftb << "\t\ttorandom = {$random} % 10;\n";
            ftb << "\t\tif(torandom > 4)\n";
            ftb << "\t\t\t" << port_name << "_tb" << " = " << v << " + {$random} % " << to_string((1 << width_to_size(port_width)) - v) << ";\n";
        }
    }
    else
        ftb << "\t\t" << port_name << "_tb" << " = {$random} % " << to_string(1 << width_to_size(port_width)) << ";\n";
}

void do_it_random(std::ofstream& ftb, const ParsedModule* module, int itr) {

    unordered_map<string, int> mp_if, mp_case;

    map_if_conditions(module, mp_if);
    map_case_conditions(module, mp_case);


    ftb << "integer torandom;\ninitial\nbegin\n";

    initialize(ftb, module);

    ftb << "\n";
    monitor(ftb, module);
    ftb << "\n";

    ftb << "\t#100;\n\n";
    ftb << "\trepeat(" << to_string(itr) << ") \n" << "\tbegin\n";

    string port_name, port_width;
    for (int i = 0; i < module->input_ports.size(); i++) {

        port_name = module->input_ports[i].first;
        port_width = module->input_ports[i].second;

        if (mp_if[port_name] >= 0)
            handle_if_conditions(ftb, module, mp_if[port_name], module->input_ports[i]);
        else if (mp_case[port_name] >= 0)
            handle_case_conditions(ftb, module, mp_case[port_name], module->input_ports[i]);
        else
            ftb << "\t\t" << port_name << "_tb" << " = {$random} % " << to_string(1 << width_to_size(port_width)) << ";\n";
    }
    if (module->is_clocked)
        ftb << "\n\t\t#25; \n";
    ftb << "\tend\n\n\t$stop;\nend";
    return;
}
