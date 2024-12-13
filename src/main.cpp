#include <time.h> 
#include <chrono>

#include "include/vm.h"
#include "compiler/include/lexer.h"

int main()
{
    
    Lexer lexer("int a := 5 string b := \"faa zz5\" if else while ", true);
    lexer.make_tokens();

    /*FemiraVirtualMachine vm;

    auto start = chrono::high_resolution_clock::now();

    vm.runf_bytecode({
        Instruction(Opcode(OP_PUSHV), new Integer(0x1)), // push address
        Instruction(Opcode(OP_WRITE_DATA), new Function({
            Instruction(Opcode(OP_PUSHV), new Integer(0x2)), // push address
            Instruction(Opcode(OP_WRITE_DATA), new Integer(15)), // write value to this address

            Instruction(Opcode(OP_READ_DATA), new Integer(0x2)), // read from address and push value to stack
            Instruction(Opcode(OP_RETURN)),
        }, 0)), // write fn to this address
        Instruction(Opcode(OP_READ_DATA), new Integer(0x1)), // get fn by address

        Instruction(Opcode(OP_PUSHV), new Double(1.5)),
        Instruction(Opcode(OP_WAIT)),
        
        Instruction(Opcode(OP_CALL)), // call fn
        Instruction(Opcode(OP_PRINT)) // print result
    }, true);

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed_seconds = end - start;

    cout << "Elapsed time: " << elapsed_seconds.count() << " seconds" << endl;*/

    return 0;
}