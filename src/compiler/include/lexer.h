#pragma once

#include <string>
#include <vector>
#include <map>
#include <regex>

using namespace std;

enum TokenType 
{
    IDENTIFIER,

    STRING,
    DIGIT,
    NIL,
    TRUE,
    FALSE,

    PLUS,
    MINUS,
    ASTERISK,
    SLASH,
    ASSIGN,

    ANNOTATE,
    TYPE,

    EQ,
    NOTEQ,

    BIGGER,
    SMALLER,

    BIGGER_OR_EQ,
    SMALLER_OR_EQ,

    AND,
    OR,
    
    NOT,

    BEGIN,
    END,

    IF,
    ELSE,

    WHILE,
    FOR,

    FUNCTION,

    LPAREN,
    RPAREN,

    LSQPAREN,
    RSQPAREN,

    COMMA,
    DOT,
    ARROW,

    RETURN,
    PRINT,

    WHITESPACE,
    SEMICOLON,
    NEWLINE,

    WAIT,
};  

struct Token 
{
    TokenType type;
    string value;
    int position;

    Token(TokenType type, string value, int position) { this->position = position; this->value = value; this->type = type; };
};

class Lexer 
{
    private:
        vector<Token*> tokens;
        string code;
        bool trace;
        int position;

        Token* next_token();
    public:

        Lexer(string code, bool trace);

        vector<Token*> make_tokens();
};