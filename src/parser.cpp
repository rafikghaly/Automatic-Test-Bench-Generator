#include <iostream>
#include <vector>
#include <regex>
#include <fstream>
#include "parsedModule.h"

//#define print

using namespace std;


string trim(const string& str) { // trim space
    string::size_type first = str.find_first_not_of(' ');
    if (first == string::npos) {
        return "";
    }
    string::size_type last = str.find_last_not_of(' ');
    return str.substr(first, last - first + 1);
}

void is_clocked(ParsedModule* module) {
    int i;
    module->is_clocked = 0;
    for (int i = 0; i < module->input_ports.size(); i++) {
        if (module->input_ports[i].first == "clk" ||
            module->input_ports[i].first == "CLK" ||
            module->input_ports[i].first == "clock" ||
            module->input_ports[i].first == "CLOCK") {
            module->is_clocked = 1;
            module->clock_name = module->input_ports[i].first;
            module->input_ports.erase(module->input_ports.begin() + i);
            break;
        }
    }

}

void pr(ParsedModule* module) {
    for (auto& e : module->input_ports)
        cout << e.first << ' ' << e.second << endl;
    for (auto& e : module->output_ports)
        cout << e.first << ' ' << e.second << endl;
    for (auto& e : module->if_statements)
        cout << e.identifier << ' ' << e.condition << ' ' << e.value << endl;
    for (auto& e : module->case_statements) {
        cout << e.identifier << endl;
        for (auto& c : e.condition)
            cout << c << endl;
    }
}


void parse(ParsedModule* module, string file) {
    if (file[0] == '\"') file = file.substr(1, file.size());
    if (file[file.size() - 1] == '\"') file = file.substr(0, file.size() - 1);
    string code;
    fstream txt;
    bool is_default = false;  // msut be in struct
    txt.open(file, ios::in);
    if (txt.is_open()) { //checking whether the file is open
        string tp;
        vector<string> labels;
        string cstat;
        while (getline(txt, tp)) {

            int pos = tp.find(";");
            if (pos > 0)
                tp.erase(pos);
            tp = trim(tp);

            regex inputWire_port_regex("(input wire|input)\\s*(\\[\\d+:\\d+\\])?\\s*(\\w+)");

            regex inputReg_port_regex("input reg\\s*(\\[\\d+:\\d+\\])?(\\w+)");

            regex inoutWire_port_regex("(inout wire |inout)\\s*(\\[\\d+:\\d+\\])?(\\w+)");

            regex inoutReg_port_regex("inout reg\\\s*(\\[\\d+:\\d+\\])?(\\w+)");

            regex outputWire_port_regex("(output wire|output)\\s*(\\[\\d+:\\d+\\])?\\s*(\\w+)");

            regex outputReg_port_regex("output reg\\s*(\\[\\d+:\\d+\\])?\\s*(\\w+)");

            regex reg_port_regex("reg\\s*(\\[\\d+:\\d+\\])?\\s*(\\w+)");

            regex module_port_regex(R"(module\s+([a-zA-Z_][a-zA-Z_0-9]*))");
            // (wire|input\s*(?!reg)
            regex wire_port_regex("wire\\s*(\\[\\d+:\\d+\\])?\\s*(\\w+)");

            regex ContAssignment_port_regex("assign\\s+(\\w+)\\s*=\\s*(\\w+)");

            regex inOfAlways_port_regex("(\\w+)\\s*@\\((\\w+)\\s+(\\w+)\\s*(or|,)?\\s*(\\w+)?\\)");

            regex inOfAlways_Allinputs_port_regex("(\\w+)\\s*@\\(\\*\\)");

            regex condOfIf_port_regex("(\\w+)\\((\\w+)\\s*(>|>=|<|<=|==|!=)?\\s*(\\w+|\\w+\\'\\w+)?\\)");

            regex switch_port_regex("(\\w+)\\s*\\((\\w+)\\)");

            regex cases_port_regex("(\\w+)\\'(\\w+)");

            regex statement_port_regex("(?!if)(\\w+)\\s*(<=|=)\\s*(\\w+)");

            regex statement2_port_regex("(?!if)(\\w+)\\s*(<=|=)\\s*(\\w+)\\'(\\w+)");

            sregex_iterator inoutReggg(tp.begin(), tp.end(), inoutReg_port_regex);
            sregex_iterator inouttWire(tp.begin(), tp.end(), inoutWire_port_regex);
            sregex_iterator reg(tp.begin(), tp.end(), reg_port_regex);
            sregex_iterator inWire(tp.begin(), tp.end(), inputWire_port_regex);
            sregex_iterator outReg(tp.begin(), tp.end(), outputReg_port_regex);
            sregex_iterator outWire(tp.begin(), tp.end(), outputWire_port_regex);
            sregex_iterator modulou(tp.begin(), tp.end(), module_port_regex);
            sregex_iterator wire(tp.begin(), tp.end(), wire_port_regex);
            sregex_iterator contAssign(tp.begin(), tp.end(), ContAssignment_port_regex);
            sregex_iterator inAlwys(tp.begin(), tp.end(), inOfAlways_port_regex);
            sregex_iterator condOfIf(tp.begin(), tp.end(), condOfIf_port_regex);
            sregex_iterator swt4(tp.begin(), tp.end(), switch_port_regex);
            sregex_iterator casseessss(tp.begin(), tp.end(), cases_port_regex);
            sregex_iterator statements(tp.begin(), tp.end(), statement_port_regex);
            sregex_iterator statements2(tp.begin(), tp.end(), statement2_port_regex);
            sregex_iterator Always2(tp.begin(), tp.end(), inOfAlways_Allinputs_port_regex);
            sregex_iterator end;

            if (tp.find("default")) {
                is_default = true;
            }
            if (condOfIf != end) {
                smatch match = *condOfIf;
                IfStatement ifstat = { match[2],match[3],match[4] };
                if (ifstat.condition == "")
                    ifstat.condition = "==", ifstat.value = "1";
                module->if_statements.push_back(ifstat);
#ifdef print
                cout << "if condition: " << endl << "identifier: " << ifstat.identifier << endl << "operator: " << match[3] << endl << "value: " << match[4] << endl;
#endif
                ++(condOfIf);
                continue;
            }
            if (contAssign != end) {
                smatch match = *contAssign;
#ifdef print
                cout << "cont assignment: " << endl;
                cout << "identifier = " << match[1] << endl;
                cout << "Value = " << match[2] << endl;
#endif
                VerilogStatement vs = { Type::ASSIGNMENT, match[1], match[2] };
                module->statements.push_back(vs);
                ++(contAssign);
                continue;
            }
            if (statements2 != end) {
                smatch match = *statements2;
#ifdef print
                cout << "identifier: " << match[1] << endl << " operator: " << match[2] << endl << "value: " << match[3] << "'" << match[4] << endl;
#endif
                ++(statements2);
                continue;
            }
            if (statements != end) {
                smatch match = *statements;
#ifdef print
                cout << "identifier: " << match[1] << endl << " operator: " << match[2] << endl << "value: " << match[3] << endl;
#endif
                ++(statements);
                continue;
            }

            if (outReg != end) {
                smatch match = *outReg;
                string width = "";
                if (match[2].matched) {
#ifdef print
                    cout << "Out reg name: " << match[2] << endl;
#endif
                }
                if (match[1].matched) {
                    width = match[1];
#ifdef print
                    cout << "witdh: " << match[1] << endl;
#endif
                }
                module->output_ports.push_back(make_pair(match[2], width));
                continue;
            }
            if (inoutReggg != end) {
                smatch match = *inoutReggg;
                if (match[2].matched) {
#ifdef print
                    cout << "Inout reg name: " << match[2] << endl;
#endif
                }

                if (match[1].matched) {
#ifdef print
                    cout << "witdh: " << match[1] << endl;
#endif
                }
                continue;
            }


            while (inouttWire != end) {
                smatch match = *inouttWire;
                if (match[3].matched) {
#ifdef print
                    cout << "Inout Wire name: " << match[3] << endl;
#endif
                }

                if (match[2].matched) {
#ifdef print
                    cout << "witdh: " << match[2] << endl;
#endif
                }
                ++(inouttWire);
            }
            while (casseessss != end) {
                smatch match = *casseessss;
#ifdef print
                cout << "cases:  " << match[1] << "'" << match[2] << endl; // concat all o them
#endif
#ifdef CASE
                string st = match[1] + "'" + match[2];
                labels.push_back(st);
#endif
                ++(casseessss);
            }
            while (swt4 != end) {
                smatch match = *swt4;
#ifdef print
                cout << "switch base: " << match[2] << endl;
#endif
#ifdef CASE
                if (cstat != "") {
                    module->case_statements.push_back(new CaseStatement{ cstat,new vector<string>(labels),0 });
                    labels.erase(labels.begin(), labels.end());
                }
                cstat = match[2];
#endif
                ++(swt4);
            }
            while (inAlwys != end) {
                smatch match = *inAlwys;
#ifdef print
                cout << "************Always inputs: " << endl << "type: " << match[2] << " " << " sensitivity: " << match[3] << " " << match[4] << " " << match[5] << endl;
#endif
                ++(inAlwys);
            }
            while (Always2 != end) {
                smatch match = *Always2;
#ifdef print
                cout << "************Always All input: " << "sensitivity : " << "*" << endl;
#endif
                ++(Always2);
            }



            while (inWire != end) {
                smatch match = *inWire;
                string width;
                if (match[3].matched) {
#ifdef print
                    cout << "Input wire name: " << match[3] << endl;
#endif
                }

                if (match[2].matched) {
                    width = match[2];
#ifdef print
                    cout << "witdh: " << match[2] << endl;
#endif
                }
                module->input_ports.push_back(make_pair(match[3], width));
                ++(inWire);
            }

            while (reg != end) {
                smatch match = *reg;
                if (match[2].matched) {
#ifdef print
                    cout << " internal reg name: " << match[2] << endl;
#endif
                }

                if (match[1].matched) {
#ifdef print
                    cout << "witdh: " << match[1] << endl;
#endif
                }
                ++(reg);
            }
            while (modulou != end) {
                smatch match = *modulou;
                module->module_name = match[1];
#ifdef print
                cout << "modulou name: " << match[1] << endl;
#endif
                ++(modulou);
            }
            while (wire != end) {
                smatch match = *wire;
                if (match[2].matched) {
#ifdef print
                    cout << " internal wire name: " << match[2] << endl;
#endif
                }

                if (match[1].matched) {
#ifdef print
                    cout << "witdh: " << match[1] << endl;
#endif
                }

                ++(wire);
            }


        }
        pr(module);
        is_clocked(module);
        txt.close(); //close the file object.
    }
    //    CaseStatement cs = {"Operation", {"2'b00", "2'b01","2'b10" ,"default"},1};
    //    module->case_statements.push_back(cs);


    //   //Open the file
    //    std::ifstream txt1(file, std::ios::in);
    //    if (txt1.is_open()) {
    //        //Read the file into a string
    //        std::string file_contents;
    //        std::string line;
    //        while (std::getline(txt1, line)) {
    //            file_contents += line + "\n";
    //        }
    //        //cout << file_contents;
    //         std::string input = "case (sel) \n"
    //                      "  2'b00: out = a;\n"
    //                      "  2'b01: out = b;\n"
    //                      "  default: out = c;\n"
    //                      "endcase\n"
    //                      "case (sel) \n"
    //                      "  2'b10: out = d;\n"
    //                      "  2'b11: out = e;\n"
    //                      "  default: out = f;\n"
    //                      "endcase";
    //         //Create a regular expression object
    //  std::regex rx("case\\s*\\(\\s*(.+?)\\s*\\)\s*([\\s\\S]+?)(?:default\\s*:\\s*([\\s\\S]+?)\\s*;||endcase)");        //Use the regular expression object to search for multiple matches in the input string
    //
    //        for (std::sregex_iterator i = std::sregex_iterator(file_contents.begin(), file_contents.end(), rx);i != std::sregex_iterator(); ++i) {
    //            std::smatch match = *i;
    //            string str;
    //            std::vector<string> labels;
    //            str = match[0];
    //            cout << str << endl;
    //            str = match[1];
    //            cout << str << endl;
    //            str = match[2];
    //            cout << str << endl;
    ////            str = match[0];
    ////            cout << str << endl;
    ////            std::regex label_rx(R"((?<case_label>\d+'[bhd]))");
    ////            for (std::sregex_iterator j = std::sregex_iterator(str.begin(), str.end(), label_rx);
    ////                 j != std::sregex_iterator(); ++j) {
    ////              std::smatch m = *j;
    ////              labels.push_back(m[0]);
    ////            }
    //
    //
    //        }
    //
    //    }
}

