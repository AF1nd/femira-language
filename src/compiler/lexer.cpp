#include <string>
#include <vector>
#include <cctype>
#include <iostream>

#include "include/lexer.h"

using namespace std;

bool is_char_quote(char c)
{
    if (c == '\'' || c == '\"') return true;
    return false;
}

char* get_char_at_position(string code, int position)
{
    if (position < code.size()) return new char(code.at(position));

    return nullptr;
}

Lexer::Lexer(string code, bool trace) 
{
    this->trace = trace;
    this->code = code;
    this->position = 0;
    this->line = 1;
}

vector<Token*> Lexer::make_tokens() 
{
    while (this->position < this->code.size()) {
        Token* token = this->next_token();
        if (token == nullptr) break;
        
        this->tokens.push_back(token);
        this->position++;
    }

    if (this->trace) {
        for (Token* token_ptr: this->tokens) {
            cout << token_ptr->type << " | " << token_ptr->value << endl;
        }
    }

    return this->tokens;
}

Token* Lexer::next_token()
{
    int current_position = this->position;
    int start_position = this->position;

    char current_char = this->code.at(start_position);
    char* next_char = get_char_at_position(this->code, start_position + 1);


    if (current_char == ' ') return new Token(WHITESPACE, " ", start_position);
    else if (current_char == '\n')
    {
        this->line++;
        return new Token(NEWLINE, "\n", start_position);
    }

    if (is_char_quote(current_char)) 
    {
        string buffer;
        bool is_quote_finded = false;

        for (size_t i = start_position + 1; i < this->code.size(); ++i) 
        {
            current_position = i;
            this->position = i;

            current_char = this->code.at(current_position);

            if (is_char_quote(current_char))
            {
                is_quote_finded = true;
                break;
            } else buffer.push_back(current_char);
        }

        if (!is_quote_finded) throw runtime_error("String must have two quotes | Line: " + to_string(this->line));

        return new Token(STRING, buffer, start_position);
    } else if (isdigit(current_char))
    {
        string buffer;
        for (size_t i = start_position; i < this->code.size(); ++i) 
        {
            current_position = i;
            this->position = i;

            current_char = this->code.at(current_position);

            if (isdigit(current_char) || current_char == '.') buffer.push_back(current_char);
            else break;
        }

        return new Token(DIGIT, buffer, start_position);
    } else if (current_char == ':')
    {
        if (next_char != nullptr && *next_char == '=')
        {
            this->position++;
            return new Token(ASSIGN, ":=", start_position);
        }
    } else if (current_char == '?')
    {
        if (next_char != nullptr && *next_char == '=')
        {
            this->position++;
            return new Token(EQ, "?=", start_position);
        }
    } else if (current_char == '!')
    {
        if (next_char != nullptr && *next_char == '=')
        {
            this->position++;
            return new Token(NOTEQ, "!=", start_position);
        }

        return new Token(NOT, "!", start_position);
    } else if (current_char == '>')
    {
        if (next_char != nullptr && *next_char == '=')
        {
            this->position++;
            return new Token(BIGGER_OR_EQ, ">=", start_position);
        }

        return new Token(BIGGER, ">", start_position);
    } else if (current_char == '<')
    {
        if (next_char != nullptr && *next_char == '=')
        {
            this->position++;
            return new Token(SMALLER_OR_EQ, "<=", start_position);
        }

        return new Token(SMALLER, "<", start_position);
    } else if (current_char == '-')
    {
        if (next_char != nullptr && *next_char == '>')
        {
            this->position++;
            return new Token(ARROW, "->", start_position);
        } 

        return new Token(MINUS, "-", start_position);
    } 

    else if (current_char == '+') return new Token(PLUS, "+", start_position);
    else if (current_char == '/') return new Token(SLASH, "/", start_position);
    else if (current_char == '*') return new Token(ASTERISK, "*", start_position); 

    else if (current_char == '{')return new Token(BEGIN, "{", start_position); 
    else if (current_char == '}') return new Token(END, "}", start_position);

    else if (current_char == '(')return new Token(LPAREN, "(", start_position); 
    else if (current_char == ')') return new Token(RPAREN, ")", start_position);

    else if (current_char == '.') return new Token(DOT, ".", start_position); 
    else if (current_char == ',') return new Token(COMMA, ",", start_position);

    else
    {
        string buffer;
        for (size_t i = start_position; i < this->code.size(); ++i) 
        {
            current_position = i;
            this->position = i;

            current_char = this->code.at(current_position);

            if (current_char == ' ') break;

            buffer.push_back(current_char);

            if (buffer == "fn") return new Token(FUNCTION, buffer, start_position);
            else if (buffer == "return") return new Token(RETURN, buffer, start_position);
            else if (buffer == "print") return new Token(PRINT, buffer, start_position);

            else if (buffer == "if") return new Token(IF, buffer, start_position);
            else if (buffer == "else") return new Token(ELSE, buffer, start_position);

            else if (buffer == "while") return new Token(WHILE, buffer, start_position);
            else if (buffer == "for") return new Token(FOR, buffer, start_position);
        }

        return new Token(IDENTIFIER, buffer, start_position);
    }

    return nullptr;
}