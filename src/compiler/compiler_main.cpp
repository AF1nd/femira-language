#include <vector>
#include <string>

#include "include/parser.h"
#include "../include/vm.h"
#include "include/compiler_main.h"

using namespace std;

vector<Instruction> CompilerMain::get_generated_bytecode()
{
    return this->generated;
}

void CompilerMain::node_to_bytecode(AstNode* node)
{
    if (IdentifierNode* identifier = dynamic_cast<IdentifierNode*>(node))
    {
        this->generated.push_back(
            Instruction(Opcode(OP_READ_DATA), new String(identifier->token->value))
        );
    } else if (LiteralNode* literal = dynamic_cast<LiteralNode*>(node)) 
    {
        Object* data;

        TokenType token_type = literal->token->type;
        string token_value = literal->token->value;

        switch (token_type)
        {
            case DIGIT:
                {
                    bool integer = true;
                    if (token_value.find(".") != string::npos) integer = false;

                    if (!integer) data = new Double(stod(token_value));
                    else data = new Integer(stoi(token_value));
                }
                break;   
            case TRUE:
                {
                    data = new Boolean(true);
                }
                break;
            case FALSE:
                {
                    data = new Boolean(false);
                }
                break;
            case NIL:
                {
                    data = new Null();
                }
                break;
            case STRING:
                {
                    data = new String(token_value);
                }
                break;
            default:
                break;
        }

        this->generated.push_back(Instruction(Opcode(OP_PUSHV), data));
    } else if (CallNode* call = dynamic_cast<CallNode*>(node)) 
    {
        for (AstNode* argument: call->with_args)
        {   
            this->node_to_bytecode(argument);
        }

        this->node_to_bytecode(call->to_call);

        this->generated.push_back(Instruction(OP_CALL));
    } else if (FunctionNode* function = dynamic_cast<FunctionNode*>(node))
    {
        CompilerMain compiler;
        compiler.node_to_bytecode(function->block);

        Function function_object(compiler.get_generated_bytecode(), function->needed_arguments.size());

        int i = 0;
        for (IdentifierNode* argument: function->needed_arguments) function_object.args_ids[i] = argument->token->value;

        this->generated.push_back(Instruction(Opcode(OP_PUSHV), new Function(function_object)));
        this->generated.push_back(Instruction(Opcode(OP_WRITE_DATA), new String(function->id->token->value)));
    } else if (UnaryOperationNode* unary = dynamic_cast<UnaryOperationNode*>(node))
    {
        TokenType token_type = unary->token->type;

        this->node_to_bytecode(unary->operand);

        switch (token_type)
            {
            case PRINT:
                {
                    this->generated.push_back(Instruction(OP_PRINT));
                }
                break;
            case RETURN:
                {
                    this->generated.push_back(Instruction(OP_RETURN));
                }
                break;
            default:
                break;
        }
    } else if (ParenthisizedNode* parenthisized = dynamic_cast<ParenthisizedNode*>(node))
    {
        this->node_to_bytecode(parenthisized->wrapped);
    } else if (IfNode* if_statement = dynamic_cast<IfNode*>(node))
    {
        this->node_to_bytecode(if_statement->condition);

        CompilerMain compiler1;
        compiler1.node_to_bytecode(if_statement->success_block);

        CompilerMain compiler2;
        compiler2.node_to_bytecode(if_statement->fail_block);

        Bytecode success_bytecode = compiler1.get_generated_bytecode();
        Bytecode fail_bytecode = compiler2.get_generated_bytecode();

        if (!fail_bytecode.empty())
        {
            success_bytecode.push_back(Instruction(Opcode(OP_JUMP), new Integer(fail_bytecode.size())));
        }

        this->generated.push_back(Instruction(Opcode(OP_JUMPIFNOT), new Integer(success_bytecode.size())));

        for (Instruction instr: success_bytecode)
        {
            this->generated.push_back(instr);
        }

        for (Instruction instr: fail_bytecode)
        {
            this->generated.push_back(instr);
        }
    } else if (WhileNode* while_node = dynamic_cast<WhileNode*>(node))
    {
        int old = this->generated.size();

        this->node_to_bytecode(while_node->condition);

        int added = this->generated.size() - old;

        CompilerMain compiler1;
        compiler1.node_to_bytecode(while_node->block);

        Bytecode bytecode = compiler1.get_generated_bytecode();
        bytecode.push_back(Instruction(Opcode(OP_JUMP), new Integer(-bytecode.size() + -5)));

        this->generated.push_back(Instruction(Opcode(OP_JUMPIFNOT), new Integer(bytecode.size())));

        for (Instruction instr: bytecode)
        {
            this->generated.push_back(instr);
        }
    } else if (ArrayNode* array = dynamic_cast<ArrayNode*>(node))
    {
        this->temp_array_index++;

        string temp_array_address = "tempnewarray" + to_string(this->temp_array_index);

        this->generated.push_back(Instruction(Opcode(OP_NEWARRAY)));
        this->generated.push_back(Instruction(Opcode(OP_WRITE_DATA), new String(temp_array_address)));

        int index = 0;
        for (AstNode* element: array->elements)
        {
            this->generated.push_back(Instruction(Opcode(OP_PUSHV), new Integer(index)));

            this->node_to_bytecode(element);

            this->generated.push_back(Instruction(Opcode(OP_READ_DATA), new String(temp_array_address)));
            this->generated.push_back(Instruction(Opcode(OP_SETINDEX)));

            index++;
        }

        this->generated.push_back(Instruction(Opcode(OP_READ_DATA), new String(temp_array_address)));

        this->generated.push_back(Instruction(Opcode(OP_PUSHV), new Null()));
        this->generated.push_back(Instruction(Opcode(OP_WRITE_DATA), new String(temp_array_address)));
    } else if (ObjectNode* object = dynamic_cast<ObjectNode*>(node))
    {
        this->temp_object_index++;

        string temp_object_address = "tempnewobject" + to_string(this->temp_object_index);

        this->generated.push_back(Instruction(Opcode(OP_NEWOBJECT)));
        this->generated.push_back(Instruction(Opcode(OP_WRITE_DATA), new String(temp_object_address)));

        for (AstNode* field: object->fields)
        {
            if (BinaryOperationNode* assignment = dynamic_cast<BinaryOperationNode*>(field))
            {
                if (assignment->operator_token->type == ASSIGN)
                {
                    IdentifierNode* identifier = dynamic_cast<IdentifierNode*>(assignment->left_operand);
                    if (!identifier) throw runtime_error("Compilation error! Assignment left operand can be only identifier");

                    this->generated.push_back(Instruction(Opcode(OP_PUSHV), new String(identifier->token->value)));

                    this->node_to_bytecode(assignment->right_operand);

                    this->generated.push_back(Instruction(Opcode(OP_READ_DATA), new String(temp_object_address)));
                    this->generated.push_back(Instruction(Opcode(OP_SETINDEX)));
                }
            }
        }

        this->generated.push_back(Instruction(Opcode(OP_READ_DATA), new String(temp_object_address)));

        this->generated.push_back(Instruction(Opcode(OP_PUSHV), new Null()));
        this->generated.push_back(Instruction(Opcode(OP_WRITE_DATA), new String(temp_object_address)));
    } else if (IndexationNode* indexation = dynamic_cast<IndexationNode*>(node))
    {
        this->node_to_bytecode(indexation->index);
        this->node_to_bytecode(indexation->where);

        this->generated.push_back(Instruction(Opcode(OP_READINDEX)));
    } else if (BinaryOperationNode* binary = dynamic_cast<BinaryOperationNode*>(node))
    {
        TokenType operator_type = binary->operator_token->type;
        AstNode* left_operand = binary->left_operand;
        if (IdentifierNode* identifier = dynamic_cast<IdentifierNode*>(left_operand))
        {
            if (operator_type == ASSIGN)
            {
                this->node_to_bytecode(binary->right_operand);
                this->generated.push_back(Instruction(Opcode(OP_WRITE_DATA), new String(identifier->token->value))); 
                
                return;
            };
        } else if (IndexationNode* indexation = dynamic_cast<IndexationNode*>(left_operand))
        {
            if (operator_type == ASSIGN)
            {
                this->node_to_bytecode(indexation->index);
                this->node_to_bytecode(binary->right_operand);
                this->node_to_bytecode(indexation->where);

                this->generated.push_back(Instruction(Opcode(OP_SETINDEX))); 
                
                return;
            };
        }

        this->node_to_bytecode(binary->left_operand);
        this->node_to_bytecode(binary->right_operand);

        switch (binary->operator_token->type)
        {
            case PLUS:
                {
                    this->generated.push_back(Instruction(Opcode(OP_ADD)));
                }
                break;
            case MINUS:
                {
                    this->generated.push_back(Instruction(Opcode(OP_SUB)));
                }
                break;
            case ASTERISK:
                {
                    this->generated.push_back(Instruction(Opcode(OP_MUL)));
                }
                break;
            case SLASH:
                {
                    this->generated.push_back(Instruction(Opcode(OP_DIV)));
                }
                break;
            case EQ:
                {
                    this->generated.push_back(Instruction(Opcode(OP_EQ)));
                }
                break;
            case NOTEQ:
                {
                    this->generated.push_back(Instruction(Opcode(OP_NOTEQ)));
                }
                break;
            case AND:
                {
                    this->generated.push_back(Instruction(Opcode(OP_AND)));
                }
                break;
            case OR:
                {
                    this->generated.push_back(Instruction(Opcode(OP_OR)));
                }
                break;
            case BIGGER:
                {
                    this->generated.push_back(Instruction(Opcode(OP_BIGGER)));
                }
                break;
            case SMALLER:
                {
                    this->generated.push_back(Instruction(Opcode(OP_SMALLER)));
                }
                break;
            case BIGGER_OR_EQ:
                {
                    this->generated.push_back(Instruction(Opcode(OP_BIGGEROREQ)));
                }
                break;
            case SMALLER_OR_EQ:
                {
                    this->generated.push_back(Instruction(Opcode(OP_SMALLEROREQ)));
                }
                break;
            default:
                break;
        }
    } else if (BlockNode* block = dynamic_cast<BlockNode*>(node))
    {
        for (AstNode* node: block->nodes) this->node_to_bytecode(node);
    }
}