#ifndef lexer_hpp
#define lexer_hpp
#include <iostream>
#include <vector>
#include "token.hpp"
#include "lexer_matrix.h"
using namespace std;

class Lexer {
    private:
        bool shouldSkip(char ch);
        Token makeLexToken(TKSymbol symbol, char* text, int length);
        Token nextToken(char* input);
    public:
        Lexer();
        vector<Token> tokenizeInput(char* input);
};

Lexer::Lexer() { }

Token Lexer::makeLexToken(TKSymbol symbol, char* text, int length) {
    return Token(symbol, string(text, length));
}

Token Lexer::nextToken(char* input) {
    int state = 1;
    int last_match = 0;
    int match_len = 0;
    for (char* p = input; *p; *p++) {
        state = matrix[state][*p];
        if (state > 0 && accept[state] > -1) {
            last_match = state;
            match_len = (p-input)+1;
        }
        if (state < 1) {
            break;
        }
    }
    if (last_match == 0) {
        return {TK_EOI};
    }
    return makeLexToken((TKSymbol)accept[last_match], input, match_len);
}

bool Lexer::shouldSkip(char c) {
    return (c == ' ' || c == '\t' || c == '\r' || c == '\n'); 
}

vector<Token> Lexer::tokenizeInput(char* input) {
    vector<Token> tokens;
    for (int i = 0; input[i] != '\0';) { 
        while (shouldSkip(input[i])) i++;
        Token next = nextToken(input+i);
        if (next.getSymbol() != TK_EOI) {
            cout<<"<"<< next.getSymbol()<<", "<<next.getString()<<">\n";
            i += next.getString().length();
            tokens.push_back(next);
        } else {
            i++;
        }
    }
    tokens.push_back(Token(TK_EOI, "<fin>"));
    return tokens;
}
#endif