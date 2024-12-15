#pragma once

#include <iostream>
#include <vector>

#include "lexer.h"

using namespace std;

struct AstNode
{
    virtual string tostring() { return "unknown node"; }
};

struct ParenthisizedNode : AstNode
{
    AstNode* wrapped;
    ParenthisizedNode(AstNode* wrapped) { this->wrapped = wrapped; };

    string tostring() override
    {
        return "[parenhisized: " + this->wrapped->tostring() + "]";
    }
};

struct IdentifierNode : AstNode
{
    Token* token;
    AstNode* type;

    IdentifierNode(Token* token, AstNode* type = nullptr) { this->token = token; this->type = type; };

    string tostring() override
    {
        return "[id: " + this->token->value + ", type: " + (this->type ? this->type->tostring() : "unknown") + "]";
    }
};

struct BinaryOperationNode : AstNode
{
    AstNode* left_operand;
    AstNode* right_operand;
    Token* operator_token;

    BinaryOperationNode(AstNode* left_operand, Token* operator_token, AstNode* right_operand) { this->operator_token = operator_token; this->left_operand = left_operand; this->right_operand = right_operand; };

    string tostring() override
    {
        return "[binary: " + this->left_operand->tostring() + " " + this->operator_token->value + " " + this->right_operand->tostring() + "]";
    }
};

struct UnaryOperationNode : AstNode
{
    Token* token;
    AstNode* operand;

    UnaryOperationNode(Token* token, AstNode* operand) { this->token = token; this->operand = operand; };

    string tostring() override
    {
        return "unary: " + this->token->value + ", operand: " + this->operand->tostring();
    }
};

struct IndexationNode : AstNode
{
    AstNode* where;
    AstNode* index;

    IndexationNode(AstNode* where, AstNode* index) { this->where = where; this->index = index; };

    string tostring() override
    {
        return "indexation in: " + this->where->tostring() + ", with index: " + this->index->tostring();
    }
};

struct ArrayNode : AstNode
{
    vector<AstNode*> elements;

    ArrayNode(vector<AstNode*> elements) { this->elements = elements; };

    string tostring() override
    {
        string elems_string;

        for (AstNode* element: this->elements)
        {
            elems_string += element->tostring() + " ";
        };

        return "[array: " + elems_string + "]";
    }
};

struct ObjectNode : AstNode
{
    vector<AstNode*> fields;

    ObjectNode(vector<AstNode*> fields) { this->fields = fields; };

    string tostring() override
    {
        string fields_string = " | ";

        for (AstNode* field: this->fields)
        {
            fields_string += field->tostring() + " | ";
        };

        return "[object: " + fields_string + "]";
    }
};

struct LiteralNode : AstNode
{
    Token* token;

    LiteralNode(Token* token) { this->token = token; };

    string tostring() override
    {
        return "[literal: " + this->token->value + "]";
    }
};

struct BlockNode : AstNode
{
    vector<AstNode*> nodes;
    BlockNode(vector<AstNode*> nodes) { this->nodes = nodes; };

    string tostring() override
    {
        return "block";
    }
};

struct IfNode : AstNode
{
    BlockNode* success_block;
    BlockNode* fail_block;
    AstNode* condition;

    IfNode(BlockNode* success_block, BlockNode* fail_block, AstNode* condition) { this->success_block = success_block; this->fail_block = fail_block; this->condition; };
};

struct WhileNode : AstNode
{
    AstNode* condition;
    BlockNode* block;

    WhileNode(AstNode* condition, BlockNode* block) { this->condition = condition; this->block = block; };
};

struct FunctionNode : AstNode
{
    IdentifierNode* id;
    AstNode* return_type;

    vector<IdentifierNode*> needed_arguments;
    BlockNode* block;

    FunctionNode(IdentifierNode* id, vector<IdentifierNode*> needed_arguments, BlockNode* block, AstNode* return_type) { this->id = id; this->needed_arguments = needed_arguments, this->block = block; this->return_type = return_type; };

    string tostring() override
    {
        string args_string;

        for (AstNode* argument: this->needed_arguments)
        {
            args_string += argument->tostring() + " ";
        };

        return "function " + this->id->token->value + ", returns: " + this->return_type->tostring() + ", args: " + args_string;
    }
};

struct CallNode : AstNode
{
    AstNode* to_call;
    vector<AstNode*> with_args;

    CallNode(AstNode* to_call, vector<AstNode*> with_args) { this->to_call = to_call; this->with_args = with_args; };

    string tostring() override
    {
        string args_string;

        for (AstNode* argument: this->with_args)
        {
            args_string += argument->tostring() + " ";
        };

        return "call " + to_call->tostring() + ", args: " + args_string;
    }
};

class Parser
{
    private:
        FunctionNode* parse_function();

        CallNode* parse_call(AstNode* to_call);
        IndexationNode* parse_indexation(AstNode* where);

        BinaryOperationNode* parse_binary();
        AstNode* term();

        UnaryOperationNode* parse_unary();
        WhileNode* parse_while();
        IfNode* parse_if();
        BlockNode* parse_block();
        IdentifierNode* parse_identifier();
        LiteralNode* parse_literal();
        ObjectNode* parse_object();
        ArrayNode* parse_array();
        ParenthisizedNode* parse_parenthisized();

        Token* eat(vector<TokenType> types);
        bool match(vector<TokenType> types);

        bool is_token(vector<TokenType> types, int position);

        AstNode* subparse(AstNode* node, bool ignore_binaries = false);
        AstNode* parse_expression(bool ignore_binaries = false);

        void parser_errorf(string text);

        int position;
        int line;

        vector<Token*> tokens;
    public:
        BlockNode* make_ast(bool trace);
        Parser(vector<Token*> tokens);
};