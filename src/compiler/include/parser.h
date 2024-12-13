#pragma once

#include <iostream>
#include <vector>

#include "lexer.h"

using namespace std;

struct AstNode
{
    virtual string tostring() { return "unknown node"; }
};

struct IdentifierNode : AstNode
{
    Token* token;
    IdentifierNode* type_id;

    IdentifierNode(Token* token, IdentifierNode* type_id = nullptr) { this->token = token; this->type_id = type_id; };

    string tostring()
    {
        return "[id: " + this->token->value + ", type: " + (this->type_id ? this->type_id->token->value : "null") + "]";
    }
};

struct BinaryOperationNode : AstNode
{
    AstNode* left_operrand;
    AstNode* right_operrand;

    BinaryOperationNode(AstNode* left_operrand, AstNode* right_operrand) { this->left_operrand = left_operrand; this->right_operrand = right_operrand; };
};

struct UnaryOperationNode : AstNode
{
    AstNode* operrand;

    UnaryOperationNode(AstNode* operrand) { this->operrand = operrand; };
};

struct IndexationNode : AstNode
{
    AstNode* where;
    AstNode* index;

    IndexationNode(AstNode* where, AstNode* index) { this->where = where; this->index = index; };
};

struct ArrayNode : AstNode
{
    vector<AstNode*> elements;

    ArrayNode(vector<AstNode*> elements) { this->elements = elements; };
};

struct ObjectNode : AstNode
{
    vector<AstNode*> fields;

    ObjectNode(vector<AstNode*> fields) { this->fields = fields; };
};

struct LiteralNode : AstNode
{
    Token* token;

    LiteralNode(Token* token) { this->token = token; };
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
    IdentifierNode* return_type_id;

    vector<AstNode*> needed_arguments;
    BlockNode* block;

    FunctionNode(IdentifierNode* id, vector<AstNode*> needed_arguments, BlockNode* block, IdentifierNode* return_type_id) { this->id = id; this->needed_arguments = needed_arguments, this->block = block; this->return_type_id = return_type_id; };

    string tostring() override
    {
        string args_string;

        for (AstNode* argument: this->needed_arguments)
        {
            args_string += argument->tostring() + " ";
        };

        return "function " + this->id->token->value + ", returns: " + this->return_type_id->token->value + ", args: " + args_string;
    }
};

struct CallNode : AstNode
{
    AstNode* to_call;
    vector<AstNode*> with_args;

    CallNode(AstNode* to_call, vector<AstNode*> with_args) { this->to_call = to_call; this->with_args = with_args; };
};

class Parser
{
    private:
        FunctionNode* parse_function();

        CallNode* parse_call(AstNode* to_call);
        IndexationNode* parse_indexation(AstNode* where);

        BinaryOperationNode* parse_binary();
        UnaryOperationNode* parse_unary();
        WhileNode* parse_while();
        IfNode* parse_if();
        BlockNode* parse_block();
        IdentifierNode* parse_identifier(bool is_typised = false);
        LiteralNode* parse_literal();
        ObjectNode* parse_object();
        ArrayNode* parse_array();

        Token* eat(vector<TokenType> types);
        bool match(vector<TokenType> types);

        bool is_token(vector<TokenType> types, int position);

        AstNode* subparse(AstNode* node);
        AstNode* parse_expression();

        void parser_errorf(string text);

        int position;
        int line;

        vector<Token*> tokens;
    public:
        BlockNode* make_ast(bool trace);
        Parser(vector<Token*> tokens);
};