#pragma once

#include <vector>
#include <string>

#include "parser.h"
#include "../../include/vm.h"

using namespace std;

struct Type {
    bool is_optional;
};

struct LiteralType : Type
{
    string value;
    LiteralType(string value)
    {
        this->value = value;
    }
};

struct PointerType : Type
{
    Type* value;
    PointerType(Type* value)
    {
        this->value = value;
    }
};

struct ObjectType : Type
{
    map<string, Type*> fields;
    ObjectType(map<string, Type*> fields)
    {
        this->fields = fields;
    }
};

struct FunctionType : Type
{
    Type* returns;
    vector<Type*> args;

    FunctionType(Type* returns, vector<Type*> args)
    {
        this->returns = returns;
        this->args = args;
    }
};

struct ArrayType : Type
{
    Type* can_store;
    ArrayType(Type* can_store)
    {
        this->can_store = can_store;
    }
};

class CompilerMain
{
    private:
        vector<Instruction> generated;

        int temp_array_index = 0;
        int temp_object_index = 0;

        bool is_types_compatible(AstNode* node_1, AstNode* node_2);
        Type* get_node_type(AstNode* node);
    public:
        void node_to_bytecode(AstNode* node);
        vector<Instruction> get_generated_bytecode();
};