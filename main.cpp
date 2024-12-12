#include "src/include/vm.h"

int main()
{
    VirtualMachine vm;
    vm.runf_bytecode({
        Instruction(Opcode(PUSHV), new Integer(5)),
        Instruction(Opcode(PUSHV), new Integer(5)),
        Instruction(Opcode(ADD)),
        Instruction(Opcode(PRINT)),

        Instruction(Opcode(PUSHV), new String("Hello world")),
        Instruction(Opcode(PRINT)),

        Instruction(Opcode(PUSHV), new Double(2)),
        Instruction(Opcode(WAIT), new Double(5.5)),

        Instruction(Opcode(PUSHV), new Double(5.5)),
        Instruction(Opcode(PUSHV), new Double(1.2)),
        Instruction(Opcode(ADD)),
        Instruction(Opcode(PRINT)),
    }, true);

    return 0;
}