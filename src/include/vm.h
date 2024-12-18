#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <stack>
#include <map>

using namespace std;

enum Opcode 
{
    OP_ADD = 0x1,
    OP_SUB = 0x2,
    OP_MUL = 0x3,
    OP_DIV = 0x4,

    OP_PUSHV = 0x5,

    OP_PRINT = 0x6,
    OP_WAIT = 0x7,

    OP_WRITE_DATA = 0x8,
    OP_READ_DATA = 0x9,

    OP_RETURN = 0x10,

    OP_CALL = 0x11,

    OP_AND = 0x12,
    OP_OR = 0x13,
    OP_EQ = 0x14,
    OP_NOTEQ = 0x15,
    OP_BIGGER = 0x16,
    OP_SMALLER = 0x17,
    OP_BIGGEROREQ = 0x18,
    OP_SMALLEROREQ = 0x19,

    OP_JUMP = 0x20,
    OP_JUMPIFNOT = 0x21,

    OP_SETINDEX = 0x22,
    OP_READINDEX = 0x23,

    OP_NEWARRAY = 0x24,
    OP_NEWOBJECT = 0x25,
};

struct Object
{
    virtual string tostring() { return "unknown datatype"; };
    virtual bool is_eq(Object* with) { return false; };

    Object() = default;
};

struct Memory
{
    map<string, Object*> cells;
    vector<Memory*> sub_memories;

    Memory* parent;

    void write_data(string key, Object* value)
    {
        if (this->cells.find(key) != this->cells.end())
        {
            if (this->cells.at(key) == value) return;
        }

        this->cells[key] = value;

        for (Memory* sub_memory: this->sub_memories)
        {
            sub_memory->write_data(key, value);
        }

        Memory* d_parent = this->parent;
        while (d_parent)
        {
            if (d_parent->cells.find(key) == d_parent->cells.end()) break;
            
            d_parent->write_data(key, value);
            d_parent = d_parent->parent;
        }
    }

    Object* read_data(string key)
    {
        if (this->cells.find(key) == this->cells.end()) throw runtime_error("Cannot find value by id " + key);
        return this->cells[key];
    }
};

struct Instruction
{
    Opcode opcode;
    Object* data;

    Instruction(Opcode opcode, Object* data = nullptr) { this->opcode = opcode; this->data = data; };

    Instruction() = default;
};

using Bytecode = vector<Instruction>;

struct Null : Object 
{
    string tostring() override 
    {
        return "null (null)";
    }

    bool is_eq(Object* with) override
    {
        if (Null* null = dynamic_cast<Null*>(with)) return true;

        return false;
    }
};

struct String : Object 
{
    string data;
    String(string data) { this->data = data; };

    string tostring() override 
    {
        return this->data + " (string)";
    }

    bool is_eq(Object* with) override
    {
        if (String* string = dynamic_cast<String*>(with))
        {
            return string->data == this->data;
        }

        return false;
    }
};

struct Double;

struct Integer : Object
{
    int data;
    Integer(int data) { this->data = data; };
    
    string tostring() override 
    {
        return to_string(this->data) + " (int)";
    }

    bool is_eq(Object* with) override
    {
        if (Integer* integer = dynamic_cast<Integer*>(with))
        {
            return integer->data == this->data;
        }

        return false;
    }
};

struct Double : Object
{
    double data;
    Double(double data) { this->data = data; };

    string tostring() override 
    {
        return to_string(this->data) + " (double)";
    }

    bool is_eq(Object* with) override
    {
        if (Double* doub = dynamic_cast<Double*>(with))
        {
            return doub->data == this->data;
        }

        return false;
    }
}; 

struct Function : Object
{
    Bytecode bytecode;

    vector<string> args_ids;
    Memory* defined_in;

    Function(Bytecode bytecode, int args_number) { this->bytecode = bytecode; };

    string tostring() override 
    {
        return "(function)";
    }

    bool is_eq(Object* with) override
    {
        return false;
    }
};

struct Array : Object
{
    vector<Object*> elements;

    string tostring() override 
    {
        string elements_string;

        for (Object* obj: this->elements) elements_string += obj->tostring() + " ";

        return elements_string + " (array)";
    }
};

struct ObjectDataStructure : Object
{
    map<string, Object*> fields;

    string tostring() override
    {
        return "object (data structure)";
    }
};

struct Boolean : Object
{
    bool data;

    Boolean(bool data)
    {
        this->data = data;
    }

    string tostring() override 
    {
        return this->data ? "true (boolean)" : "false";
    }

    bool is_eq(Object* with) override
    {
        if (Boolean* boolean = dynamic_cast<Boolean*>(with)) return this->data == boolean->data;

        return false;
    }
};

class FemiraVirtualMachine 
{
    private:
        Bytecode running_bytecode;
        stack<Object*> run_stack;

        int instruction_pointer = 0;
    public:
        map<int, Bytecode> callable_bytecodes;
        
        void runf_bytecode(const Bytecode bytecode, const bool trace = false, Memory* memory = new Memory());
        void errorf(const string text);

        void push_stack(Object* data);
        Object* pop_stack();
};