#include "src/include/vm.h"

int main()
{
    VirtualMachine vm;
    vm.callableBytecodes[1] = {
        Instruction(Opcode(PUSHV), new Integer(5)),
        Instruction(Opcode(PUSHV), new Integer(5)),
        Instruction(Opcode(ADD)),
        Instruction(Opcode(PRINT)),

        Instruction(Opcode(PUSHV), new String("Hello world")),
        Instruction(Opcode(PRINT)),

        Instruction(Opcode(PUSHV), new Double(2)),
        Instruction(Opcode(WAIT)),

        Instruction(Opcode(PUSHV), new Double(5.5)),
        Instruction(Opcode(PUSHV), new Double(1.2)),
        Instruction(Opcode(ADD)),
        Instruction(Opcode(PRINT)),

        Instruction(Opcode(PUSHV), new Double(5)),
        Instruction(Opcode(PUSHV), new Double(4)),
        Instruction(Opcode(SUB)),
        Instruction(Opcode(PRINT)),
    };

    vm.runf_bytecode({
        Instruction(Opcode(CALL), new Integer(1)),
    }, true);

    return 0;
}