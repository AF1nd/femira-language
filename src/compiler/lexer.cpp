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
    } else
    {
        string buffer;
        for (size_t i = start_position; i < this->code.size(); ++i) 
        {
            current_position = i;
            this->position = i;

            current_char = this->code.at(current_position);

            if (current_char == ' ') break;

            buffer.push_back(current_char);
        }

        return new Token(IDENTIFIER, buffer, start_position);
    }

    return nullptr;
}