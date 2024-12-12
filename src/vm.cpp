#include <string>
#include <iostream>
#include <vector>
#include <stack>
#include <chrono>
#include <thread>

#include "include/vm.h"

using namespace std;

map<Opcode, string> opcode_to_string = {
    { ADD, "add" },
    { SUB, "sub" },
    { MUL, "mul" },
    { DIV, "div" },

    { PUSHV, "pushv" },

    { PRINT, "print" },
    { WAIT, "wait" },

    { RETURN, "return" },

    { WRITE_DATA, "write" },
    { READ_DATA, "read" },

    { CALL, "call" },
};

void VirtualMachine::runf_bytecode(const Bytecode bytecode, const bool trace) 
{
    this->running_bytecode = bytecode;

    if (trace)
    {
        for (Instruction instruction: bytecode)
        {
            Opcode opcode = instruction.opcode;
            Object* data = instruction.data;
            cout << opcode << ":    " + opcode_to_string[opcode] << "    " << (data != nullptr ? data->tostring() : "") << endl;
        }

        cout << "<RESULT>" << endl;
    }

    for (Instruction instruction: bytecode)
    {
        Opcode opcode = instruction.opcode;
        Object* data = instruction.data;

        switch (opcode)
        {
        case WRITE_DATA:
            {
                Object* address = this->pop_stack();
                if (Integer* integer = dynamic_cast<Integer*>(address)) {
                    this->memory[integer->data] = data;
                } else errorf("Address for data write must be a integer");
            }
            break;
        case READ_DATA:
            {
                Object* address = data; 
                
                if (Integer* integer = dynamic_cast<Integer*>(address)) {
                    if (this->memory.find(integer->data) != this->memory.end()) push_stack(this->memory.at(integer->data));
                    else this->errorf("Memory cell " + integer->tostring() + " is empty");
                } else this->errorf("Address for data read must be a integer");
            }
            break;
        case CALL:
            {
                Object* data = this->pop_stack();
                if (Function* function = dynamic_cast<Function*>(data)) {
                    runf_bytecode(function->bytecode, trace);
                } else this->errorf("No function to call in stack");
            }
            break;
        case ADD:
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
        case SUB:
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
        case MUL:
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
        case DIV:
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
        case PUSHV:
            {
                this->push_stack(data);
            }
            break;
        case RETURN:
            {
                return;
            }
            break;
        case PRINT:
            {
                string to_print = this->pop_stack()->tostring();

                cout << " ";

                for (size_t i = 0; i < to_print.length() + 4; i++)
                {
                    cout << "-";
                }

                cout << endl;

                cout << " | " + to_print + " | " << endl;

                cout << " ";
                
                for (size_t i = 0; i < to_print.length() + 4; i++)
                {
                    cout << "-";
                }

                cout << endl;
            }
            break;
        case WAIT:
            {
                Object* object = this->pop_stack();

                if (Integer* integer = dynamic_cast<Integer*>(object)) 
                {
                    this_thread::sleep_for(chrono::duration<int>(integer->data));
                } else if (Double* double_value = dynamic_cast<Double*>(object)) 
                {
                    this_thread::sleep_for(chrono::duration<double>(double_value->data));
                }
            }
            break;
        default:
            break;
        }
    }
}

void VirtualMachine::errorf(const string text) 
{
    throw runtime_error("Runtime error: " + text);
}

void VirtualMachine::push_stack(Object* data)
{
    if (data == nullptr) this->errorf("Cannot push null pointer to stack");
    this->run_stack.push(data);
}

Object* VirtualMachine::pop_stack() 
{
    Object* top = this->run_stack.top();
    if (top == nullptr) this->errorf("Stack is empty");

    this->run_stack.pop();

    return top;
}