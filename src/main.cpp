#include "include/vm.h"

int main()
{
    FemiraVirtualMachine vm;

    vm.runf_bytecode({
        Instruction(Opcode(PUSHV), new Integer(0x1)), // push address
        Instruction(Opcode(WRITE_DATA), new Function({
            Instruction(Opcode(PUSHV), new Integer(0x2)), // push address
            Instruction(Opcode(WRITE_DATA), new Integer(15)), // write value to this address

            Instruction(Opcode(READ_DATA), new Integer(0x2)), // read from address and push value to stack
            Instruction(Opcode(RETURN)),
        }, 0)), // write fn to this address
        Instruction(Opcode(READ_DATA), new Integer(0x1)), // get fn by address
        
        Instruction(Opcode(CALL)), // call fn
        Instruction(Opcode(PRINT)) // print result
    }, true);

    return 0;
}