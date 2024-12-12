#include "include/vm.h"

int main()
{
    VirtualMachine vm;
    vm.callableBytecodes[0x1] = {
        Instruction(Opcode(PUSHV), new Integer(0x1)),
        Instruction(Opcode(WRITE_DATA), new Integer(2)),

        Instruction(Opcode(READ_DATA), new Integer(0x1)),
        Instruction(Opcode(PRINT)),
    };

    vm.runf_bytecode({
        Instruction(Opcode(CALL), new Integer(0x1)),
    }, true);

    return 0;
}