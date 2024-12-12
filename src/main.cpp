#include "include/vm.h"

int main()
{
    VirtualMachine vm;

    vm.runf_bytecode({
        Instruction(Opcode(PUSHV), new Integer(0x1)), // push address
        Instruction(Opcode(WRITE_DATA), new Function({
            Instruction(Opcode(PUSHV), new Integer(0x2)), // push address
            Instruction(Opcode(WRITE_DATA), new Integer(2)), // write value to this address

            Instruction(Opcode(READ_DATA), new Integer(0x2)), // read from address and push value to stack
            Instruction(Opcode(PRINT)),

            Instruction(Opcode(PUSHV), new Integer(2)),
            Instruction(Opcode(PUSHV), new Integer(2)),

            Instruction(Opcode(ADD)),

            Instruction(Opcode(PRINT)),
        }, 0)), // write fn to this address
        Instruction(Opcode(READ_DATA), new Integer(0x1)), // get fn by address
        
        Instruction(Opcode(CALL)) // call fn
    }, true);

    return 0;
}