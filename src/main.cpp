#include "tbwriter.h"
#include "parser.h"
#include "stimulusgen.h"
#include "CONFIG.h"

int main(){
    string file_path;
    cout << "paste the address of the module file:" << endl;
    cin >> file_path;// input the path of the file
 
    ParsedModule module;
    parse(&module,file_path);
    tb_module_generator(&module);
    
    cin.get(); // system pause
}
