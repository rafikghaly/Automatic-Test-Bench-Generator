#ifndef ParsedModule_H
#define ParsedModule_H

#include <string>
#include <vector>

using namespace std;

enum class Type {
    VARIABLE,
    REGISTER,
    WIRE,
    ALWAYS,
    ASSIGNMENT,
};

struct IfStatement {
    string identifier;
    string condition;
    string value;
};

struct VerilogStatement {
    Type type;
    string identifier;
    string value;
};

struct CaseStatement {
    string identifier;
    vector<string> condition;
    bool is_default;
};

struct ParsedModule {

    string module_name;

    // dynamic array of pairs to store the port name and the bus size
    vector<pair<string, string>> input_ports;
    vector<pair<string, string>> output_ports;

    bool is_clocked;
    string clock_name;
    vector<VerilogStatement> statements;
    vector<IfStatement> if_statements;
    vector<CaseStatement> case_statements;
};

#endif // ParsedModule_H
