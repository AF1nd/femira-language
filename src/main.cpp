#include <time.h> 
#include <chrono>
#include <fstream>
#include <string>

#include "include/vm.h"
#include "compiler/include/lexer.h"
#include "compiler/include/parser.h"
#include "compiler/include/compiler_main.h"

using namespace std;

int main(int argc, char** argv)
{
    ifstream f(argv[1]);

    if (!f.is_open()) {
        cerr << "Cannot run the script" << endl;;
        return 1;
    }

    string s;
    string code;

    while (getline(f, s)) code += s;

    f.close();

    Lexer lexer(code, false);
    Parser parser(lexer.make_tokens());
    BlockNode* ast = parser.make_ast(false);

    CompilerMain compiler;

    compiler.node_to_bytecode(ast);
    Bytecode bytecode = compiler.get_generated_bytecode();

    FemiraVirtualMachine vm;

    string show_bytecode = argc > 2 ? argv[2] : "";

    vm.runf_bytecode(bytecode, show_bytecode == "yes" ? true : false);

    return 0;
}