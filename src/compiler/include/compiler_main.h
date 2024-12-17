#pragma once

#include <vector>
#include <string>

#include "parser.h"
#include "../../include/vm.h"

using namespace std;

class CompilerMain
{
    private:
        vector<Instruction> generated;

        int temp_array_index = 0;
        int temp_object_index = 0;
    public:
        void node_to_bytecode(AstNode* node);
        vector<Instruction> get_generated_bytecode();
};