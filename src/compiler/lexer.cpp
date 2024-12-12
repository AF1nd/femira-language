#include <string>
#include <vector>
#include <cctype>
#include <iostream>

#include "include/lexer.h"

using namespace std;

Lexer::Lexer(string code, bool trace) 
{
    this->trace = trace;
    this->code = code;
    this->position = 0;
}

vector<Token*> Lexer::make_tokens() 
{
    while (this->position < this->code.size()) {
        Token* token = this->next_token();
        if (token == nullptr) break;
        
        this->tokens.push_back(token);
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
    return nullptr;
}