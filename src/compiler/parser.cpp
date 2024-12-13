#include <iostream>
#include <vector>
#include <map>

#include "include/lexer.h"
#include "include/parser.h"

using namespace std;  

map<TokenType, string> token_types_names = {
    { IDENTIFIER, "identifier" },

    { STRING, "string" },
    { DIGIT, "digit" },
    { NIL, "nil" },
    { TRUE, "true" },
    { FALSE, "false" },

    { PLUS, "+" },
    { MINUS, "-" },
    { ASTERISK, "*" },
    { SLASH, "/" },
    { ASSIGN, ":="},

    { ANNOTATE, ":" },

    { EQ, "?=" },
    { NOTEQ, "!=" },

    { BIGGER, ">" },
    { SMALLER, "<" },

    { BIGGER_OR_EQ, ">=" },
    { SMALLER_OR_EQ, "<=" },
    
    { NOT, "!" },

    { BEGIN, "{" },
    { END, "}" },

    { IF, "if" },
    { ELSE, "else" },

    { WHILE, "while" },
    { FOR, "for" },

    { FUNCTION, "fn" },

    { LPAREN, "(" },
    { RPAREN, ")" },

    { LSQPAREN, "[" },
    { RSQPAREN, "]"},

    { COMMA, "," },
    { DOT, "." },
    { ARROW, "->" },

    { RETURN, "return" },
    { PRINT, "print" },

    { WHITESPACE, "whitespace" },
    { SEMICOLON, "semicolon" },
    { NEWLINE, "newline" },
};

Parser::Parser(vector<Token*> tokens)
{
    this->position = 0;
    this->line = 1;
    this->tokens = tokens;
}

bool Parser::is_token(vector<TokenType> types, int position)
{
    if (position >= this->tokens.size()) return false;

    Token* at_pos = this->tokens.at(position);
    TokenType type = at_pos->type;

    if (find(types.begin(), types.end(), type) != types.end()) return true;
    return false;
}

bool Parser::match(vector<TokenType> types)
{
    if (this->is_token(types, this->position))
    {
        this->position++;
        return true;
    }
    return false;
}

Token* Parser::eat(vector<TokenType> types)
{
    string expected_tokens;
    for (TokenType type: types) expected_tokens += token_types_names[type] + " ";

    if (this->is_token(types, this->position))
    {
        Token* token = this->tokens.at(this->position);
        this->position++;
        return token;
    }
    
    this->parser_errorf("Expected token: " + expected_tokens);

    return nullptr;
}

void Parser::parser_errorf(string text)
{
    throw runtime_error("Syntax error: " + text + ", on line: " + to_string(this->line) + ", at position: " + to_string(this->position));
}

BlockNode* Parser::make_ast(bool trace)
{
    BlockNode* ast = new BlockNode({});

    while (this->position < this->tokens.size())
    {
        if (this->match({ NEWLINE }))
        {
            this->line++;
            continue;
        }

        AstNode* expression = this->parse_expression();
        if (expression)
        {
            ast->nodes.push_back(expression);
            //this->match({ SEMICOLON });
        }
        else break;
    }

    if (trace)
    {
        for (AstNode* node: ast->nodes)
        {
            cout << node->tostring() << endl;
        }
    }

    return ast;
}

AstNode* Parser::parse_expression()
{   
    AstNode* expression;

    if (this->is_token({ FUNCTION }, this->position)) expression = this->parse_function();
    else if (this->is_token({ STRING, DIGIT, NIL, TRUE, FALSE }, this->position)) expression = this->parse_literal();
    else if (this->is_token({ IDENTIFIER }, this->position)) expression = this->parse_identifier();

    return expression;
}

FunctionNode* Parser::parse_function()
{
    this->eat({ FUNCTION });

    IdentifierNode* identifier = this->parse_identifier();

    this->eat({ LPAREN });

    vector<AstNode*> args;

    while (!this->is_token({ RPAREN }, this->position))
    {
        AstNode* argument = this->parse_identifier(true);
        if (argument)
        {
            args.push_back(argument);
            this->match({ COMMA });
        } else break;
    }
    
    this->eat({ RPAREN });

    this->eat({ ARROW });

    IdentifierNode* return_type_id = this->parse_identifier();

    BlockNode* block = this->parse_block();

    return new FunctionNode(identifier, args, block, return_type_id);
}

BlockNode* Parser::parse_block()
{
    this->eat({ BEGIN });

    vector<AstNode*> nodes;

    while (!this->is_token({ END }, this->position))
    {
        AstNode* node = this->parse_expression();
        if (node)
        {
            nodes.push_back(node);
            this->match({ SEMICOLON });
        } else break;
    }

    this->eat({ END });

    return new BlockNode(nodes);
}

LiteralNode* Parser::parse_literal()
{
    return new LiteralNode(this->eat({ STRING, DIGIT, NIL, TRUE, FALSE }));
}

IdentifierNode* Parser::parse_identifier(bool is_typised)
{
    Token* start = this->eat({ IDENTIFIER });
    
    if (!is_typised) return new IdentifierNode(start);
    else
    {
        this->eat({ ANNOTATE });
        return new IdentifierNode(start, new IdentifierNode(this->eat({ IDENTIFIER })));
    }

    return new IdentifierNode(start);
}