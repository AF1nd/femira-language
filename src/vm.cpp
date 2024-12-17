#include <string>
#include <iostream>
#include <vector>
#include <stack>
#include <chrono>
#include <thread>
#include <algorithm>

#include "include/vm.h"

using namespace std;

map<Opcode, string> opcode_to_string = {
    { OP_ADD, "add" },
    { OP_SUB, "sub" },
    { OP_MUL, "mul" },
    { OP_DIV, "div" },

    { OP_PUSHV, "pushv" },

    { OP_PRINT, "print" },
    { OP_WAIT, "wait" },

    { OP_RETURN, "return" },

    { OP_WRITE_DATA, "write_data" },
    { OP_READ_DATA, "read_data" },

    { OP_CALL, "call" },

    { OP_AND, "and" },
    { OP_OR, "or" },
    { OP_EQ, "eq" },
    { OP_NOTEQ, "noteq" },
    { OP_BIGGER, "bigger" },
    { OP_SMALLER, "smaller" },
    { OP_BIGGEROREQ, "biggeroreq" },
    { OP_SMALLEROREQ, "smalleroreq"},

    { OP_JUMP, "jump" },
    { OP_JUMPIFNOT, "jumpifnot" },

    { OP_SETINDEX, "setindex" },
    { OP_READINDEX, "readindex" },

    { OP_NEWARRAY, "newarray" },
    { OP_NEWOBJECT, "newobject" }
};

void FemiraVirtualMachine::runf_bytecode(const Bytecode bytecode, const bool trace, Memory* memory) 
{
    this->instruction_pointer = 0;
    this->running_bytecode = bytecode;

    if (trace)
    {
        cout << "<BYTECODE>" << endl;

        for (Instruction instruction: bytecode)
        {
            Opcode opcode = instruction.opcode;
            Object* data = instruction.data;
            cout << opcode << ":    " + opcode_to_string[opcode] << "    " << (data ? data->tostring() : "") << endl;
        }

        cout << "<RESULT>" << endl;
    }

    while (this->instruction_pointer < this->running_bytecode.size())
    {
        Instruction instruction = this->running_bytecode.at(this->instruction_pointer);

        Opcode opcode = instruction.opcode;
        Object* data = instruction.data;

        switch (opcode)
        {
        case OP_WRITE_DATA:
            {
                Object* address = data;
                Object* data = this->pop_stack();

                if (String* string = dynamic_cast<String*>(address)) 
                {
                    memory->write_data(string->data, data);

                    if (Function* function = dynamic_cast<Function*>(data)) function->defined_in = memory;

                    break;
                } else errorf("Address for data write must be a string");
            }
            break;
        case OP_READ_DATA:
            {
                Object* address = data; 
                
                if (String* string = dynamic_cast<String*>(address)) 
                {
                    push_stack(memory->read_data(string->data));
                } else this->errorf("Address for data read must be a string");
            }
            break;
        case OP_JUMP:
            {
                if (Integer* integer = dynamic_cast<Integer*>(data)) 
                {
                    this->instruction_pointer += integer->data;
                    break;
                }

                this->errorf("Jump error, operand must be a integer");
            }
            break;
        case OP_JUMPIFNOT:
            {
                if (Integer* integer = dynamic_cast<Integer*>(data)) 
                {
                    if (Boolean* boolean = dynamic_cast<Boolean*>(this->pop_stack()))
                    {
                        if (!boolean->data) {
                            this->instruction_pointer += integer->data;
                        }

                        break;
                    }
                }

                this->errorf("Jumpifnot error, operand must be a integer and condition must be a boolean");
            }
            break;
        case OP_CALL:
            {
                Object* data = this->pop_stack();

                if (Function* function = dynamic_cast<Function*>(data)) 
                {
                    int ip = this->instruction_pointer;

                    vector<string> args_ids;

                    Memory* defined_in_memory = function->defined_in;
                    Memory* new_memory = new Memory();

                    for (pair<string, Object*> cell: (function->defined_in->cells)) new_memory->write_data(cell.first, cell.second);

                    for (int i = 0; i < function->args_number; ++i)
                    {
                        string id = function->args_ids.at(i);
                        new_memory->write_data(id, this->pop_stack());

                        args_ids.push_back(id);
                    }    

                    defined_in_memory->sub_memories.push_back(new_memory);
                    new_memory->parent = defined_in_memory;

                    this->runf_bytecode(function->bytecode, trace, new_memory);

                    delete new_memory;

                    this->running_bytecode = bytecode;
                    this->instruction_pointer = ip;
                } else this->errorf("No function to call in stack");
            }
            break;
        case OP_ADD:
            {
                Object* obj1 = this->pop_stack();
                Object* obj2 = this->pop_stack();

                if (Integer* integer = dynamic_cast<Integer*>(obj1)) 
                {
                    if (Integer* integer2 = dynamic_cast<Integer*>(obj2)) 
                    {
                        this->push_stack(new Integer(integer->data + integer2->data));
                        break;
                    }
                } else if (Double* double_value = dynamic_cast<Double*>(obj1)) 
                {
                    if (Double* double_value2 = dynamic_cast<Double*>(obj2)) 
                    {
                        this->push_stack(new Double(double_value->data + double_value2->data));
                        break;
                    }
                }

                this->errorf("Add operation error, operands " + obj1->tostring() + " and " + obj2->tostring() + " are incompatible");
            }
            break;
        case OP_SUB:
            {
                Object* obj1 = this->pop_stack();
                Object* obj2 = this->pop_stack();

                if (Integer* integer = dynamic_cast<Integer*>(obj1)) 
                {
                    if (Integer* integer2 = dynamic_cast<Integer*>(obj2)) 
                    {
                        this->push_stack(new Integer(integer2->data - integer->data));
                        break;
                    }
                } else if (Double* double_value = dynamic_cast<Double*>(obj1)) 
                {
                    if (Double* double_value2 = dynamic_cast<Double*>(obj2)) 
                    {
                        this->push_stack(new Double(double_value2->data - double_value->data));
                        break;
                    }
                }

                this->errorf("Substract operation error, operands " + obj1->tostring() + " and " + obj2->tostring() + " are incompatible");
            }
            break;
        case OP_MUL:
            {
                Object* obj1 = this->pop_stack();
                Object* obj2 = this->pop_stack();

                if (Integer* integer = dynamic_cast<Integer*>(obj1)) 
                {
                    if (Integer* integer2 = dynamic_cast<Integer*>(obj2)) 
                    {
                        this->push_stack(new Integer(integer->data * integer2->data));
                        break;
                    }
                } else if (Double* double_value = dynamic_cast<Double*>(obj1)) 
                {
                    if (Double* double_value2 = dynamic_cast<Double*>(obj2)) 
                    {
                        this->push_stack(new Double(double_value->data * double_value2->data));
                        break;
                    }
                }

                this->errorf("Multiply operation error, operands " + obj1->tostring() + " and " + obj2->tostring() + " are incompatible");
            }
            break;
        case OP_DIV:
             {
                Object* obj1 = this->pop_stack();
                Object* obj2 = this->pop_stack();

                if (Integer* integer = dynamic_cast<Integer*>(obj1)) 
                {
                    if (Integer* integer2 = dynamic_cast<Integer*>(obj2)) 
                    {
                        this->push_stack(new Integer(integer2->data / integer->data));
                        break;
                    }
                } else if (Double* double_value = dynamic_cast<Double*>(obj1)) 
                {
                    if (Double* double_value2 = dynamic_cast<Double*>(obj2)) 
                    {
                        this->push_stack(new Double(double_value2->data / double_value->data));
                        break;
                    }
                }

                this->errorf("Divide operation error, operands " + obj1->tostring() + " and " + obj2->tostring() + " are incompatible");
            }
            break;
        case OP_PUSHV:
            {
                this->push_stack(data);
            }
            break;
        case OP_RETURN:
            return;
        case OP_AND:
            {
                Object* obj1 = this->pop_stack();
                Object* obj2 = this->pop_stack();

                if (Boolean* object1 = dynamic_cast<Boolean*>(obj1))
                {
                    if (Boolean* object2 = dynamic_cast<Boolean*>(obj2))
                    {
                        this->push_stack(new Boolean(object1->data && object2->data));
                        break;
                    }
                }

                this->errorf("Operator '&' can compare only booleans");
            }
            break;
        case OP_OR:
            {
                Object* obj1 = this->pop_stack();
                Object* obj2 = this->pop_stack();

                if (Boolean* object1 = dynamic_cast<Boolean*>(obj1))
                {
                    if (Boolean* object2 = dynamic_cast<Boolean*>(obj2))
                    {
                        this->push_stack(new Boolean(object1->data || object2->data));
                        break;
                    }
                }

                this->errorf("Operator '|' can compare only booleans");
            }
            break;
        case OP_EQ:
            {
                Object* obj1 = this->pop_stack();
                Object* obj2 = this->pop_stack();

                this->push_stack(new Boolean(obj1->is_eq(obj2)));
            }
            break;
        case OP_NOTEQ:
            {
                Object* obj1 = this->pop_stack();
                Object* obj2 = this->pop_stack();

                this->push_stack(new Boolean(!obj1->is_eq(obj2)));
            }
            break;
        case OP_BIGGER:
            {
                Object* obj1 = this->pop_stack();
                Object* obj2 = this->pop_stack();

                if (Integer* integer = dynamic_cast<Integer*>(obj1)) 
                {
                    if (Integer* integer2 = dynamic_cast<Integer*>(obj2)) 
                    {
                        this->push_stack(new Boolean(integer2->data > integer->data));
                        break;
                    }
                } else if (Double* double_value = dynamic_cast<Double*>(obj1)) 
                {
                    if (Double* double_value2 = dynamic_cast<Double*>(obj2)) 
                    {
                        this->push_stack(new Boolean(double_value2->data > double_value->data));
                        break;
                    }
                }

                this->errorf("> operator can work only with integers / doubles");
            }
            break;
        case OP_SMALLER:
            {
                Object* obj1 = this->pop_stack();
                Object* obj2 = this->pop_stack();

                if (Integer* integer = dynamic_cast<Integer*>(obj1)) 
                {
                    if (Integer* integer2 = dynamic_cast<Integer*>(obj2)) 
                    {
                        this->push_stack(new Boolean(integer2->data < integer->data));
                        break;
                    }
                } else if (Double* double_value = dynamic_cast<Double*>(obj1)) 
                {
                    if (Double* double_value2 = dynamic_cast<Double*>(obj2)) 
                    {
                        this->push_stack(new Boolean(double_value2->data < double_value->data));
                        break;
                    }
                }

                this->errorf("< operator can work only with integers / doubles");
            }
            break;
        case OP_BIGGEROREQ:
            {
                Object* obj1 = this->pop_stack();
                Object* obj2 = this->pop_stack();

                if (Integer* integer = dynamic_cast<Integer*>(obj1)) 
                {
                    if (Integer* integer2 = dynamic_cast<Integer*>(obj2)) 
                    {
                        this->push_stack(new Boolean(integer2->data >= integer->data));
                        break;
                    }
                } else if (Double* double_value = dynamic_cast<Double*>(obj1)) 
                {
                    if (Double* double_value2 = dynamic_cast<Double*>(obj2)) 
                    {
                        this->push_stack(new Boolean(double_value2->data >= double_value->data));
                        break;
                    }
                }

                this->errorf(">= operator can work only with integers / doubles");
            }
            break;
        case OP_SMALLEROREQ:
            {
                Object* obj1 = this->pop_stack();
                Object* obj2 = this->pop_stack();

                if (Integer* integer = dynamic_cast<Integer*>(obj1)) 
                {
                    if (Integer* integer2 = dynamic_cast<Integer*>(obj2)) 
                    {
                        this->push_stack(new Boolean(integer2->data <= integer->data));
                        break;
                    }
                } else if (Double* double_value = dynamic_cast<Double*>(obj1)) 
                {
                    if (Double* double_value2 = dynamic_cast<Double*>(obj2)) 
                    {
                        this->push_stack(new Boolean(double_value2->data <= double_value->data));
                        break;
                    }
                }

                this->errorf("<= operator can work only with integers / doubles");
            }
            break;
        case OP_PRINT:
            {
                Object* obj = this->pop_stack();
                string to_print = obj->tostring();

                cout << " ";

                cout << endl;

                cout << " | " + to_print + " | " << endl;

                cout << " ";

                cout << endl;
            }
            break;
        case OP_NEWARRAY:
            {
                this->push_stack(new Array());
            }
            break;
        case OP_NEWOBJECT:
            {
                this->push_stack(new ObjectDataStructure());
            }
            break;
        case OP_SETINDEX:
            {
                Object* object = this->pop_stack();
                Object* value = this->pop_stack();
                Object* index = this->pop_stack();
                
                if (Array* array = dynamic_cast<Array*>(object)) 
                {
                    if (Integer* index_integer = dynamic_cast<Integer*>(index))
                    {
                        array->elements.resize(index_integer->data + 1);
                        array->elements[index_integer->data] = value;
                        break;
                    }
                } else if (ObjectDataStructure* object_data_struct = dynamic_cast<ObjectDataStructure*>(object))
                {
                    if (String* index_string = dynamic_cast<String*>(index))
                    {
                        object_data_struct->fields[index_string->data] = value;
                        break;
                    }
                }

                this->errorf("Setindex error! Object must be a arrray or object data struct, index must be string or integer");
            }
            break;
        case OP_READINDEX:
            {
                Object* object = this->pop_stack();
                Object* index = this->pop_stack();
                
                if (Array* array = dynamic_cast<Array*>(object)) 
                {
                    if (Integer* index_integer = dynamic_cast<Integer*>(index))
                    {
                        this->push_stack(array->elements.at(index_integer->data));
                        break;
                    }
                } else if (ObjectDataStructure* object_data_struct = dynamic_cast<ObjectDataStructure*>(object))
                {
                    if (String* index_string = dynamic_cast<String*>(index))
                    {
                        this->push_stack(object_data_struct->fields.at(index_string->data));
                        break;
                    }
                }

                this->errorf("Readindex error! Object must be a arrray or object data struct, index must be string or integer");
            }
            break;
        case OP_WAIT:
            {
                Object* object = this->pop_stack();

                if (Integer* integer = dynamic_cast<Integer*>(object)) this_thread::sleep_for(chrono::duration<int>(integer->data));
                else if (Double* double_value = dynamic_cast<Double*>(object)) this_thread::sleep_for(chrono::duration<double>(double_value->data));
            }
            break;
        default:
            break;
        }

        this->instruction_pointer++;
    }
}

void FemiraVirtualMachine::errorf(const string text) 
{
    throw runtime_error("Runtime error: " + text);
}

void FemiraVirtualMachine::push_stack(Object* data)
{
    if (!data) this->errorf("Cannot push null pointer to stack");
    this->run_stack.push(data);
}

Object* FemiraVirtualMachine::pop_stack() 
{
    Object* top = this->run_stack.top();
    if (!top) this->errorf("Stack is empty");

    this->run_stack.pop();

    return top;
}