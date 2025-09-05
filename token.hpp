#ifndef token_hpp
#define token_hpp
#include <iostream>
#include "lexer_matrix.h"
using namespace std;

/*
enum TKSymbol {
    TK_NUMBER, TK_ID, TK_STRING, TK_LP, TK_RP, TK_LC, TK_RC, TK_LB, TK_RB,
    TK_ASSIGN, TK_ADD, TK_SUB, TK_MUL, TK_DIV, TK_LT, TK_EQ, TK_GT, TK_LTE, TK_GTE, TK_NEQ,
    TK_SEMI, TK_COMMA, TK_PERIOD, TK_PRINTLN, TK_LET, TK_WHILE, TK_IF, TK_ELSE,
    TK_DEF, TK_RETURN,
    TK_EOI
};
*/

class Token {
    private:
        TKSymbol symbol;
        string strval;
        int depth;
    public:
        Token(TKSymbol sym = TK_EOI, string st = "<nil>") : symbol(sym), strval(st), depth(-1) { }
        TKSymbol getSymbol() { return symbol; }
        string getString() { return strval; }
        int scopeLevel() { return depth; }
        void setScopeLevel(int level) { depth = level; }
};

#endif