#include <iostream>
#include "lexer.hpp"
#include "parser.hpp"
#include "prettyprint.hpp"
#include "interpreter.hpp"
using namespace std;

void readEvalPrintLoop() {
    bool running = true;
    Lexer lexer;
    Parser parser;
    PrettyPrinter* pp = new PrettyPrinter();
    ScopeResolver* sr = new ScopeResolver();
    Interpreter* terp = new Interpreter();
    while (running) {
        cout<<"mgcgs> ";
        string input;
        getline(cin, input);
        auto ast = parser.parse(lexer.tokenizeInput(const_cast<char*>(input.data())));
        sr->visit(ast);
        pp->visit(ast);
        terp->visit(ast);
    }
}

void execFromCmd(char* data) {
    Lexer lexer;
    Parser pp;
    auto t = pp.parse(lexer.tokenizeInput(data));
    ScopeResolver* sr = new ScopeResolver();
    sr->visit(t);
    PrettyPrinter* pv = new PrettyPrinter();
    pv->visit(t);
    Interpreter* ev = new Interpreter();
    ev->visit(t);
}

int main(int argc, char* argv[]) {
    if (argc > 1) {
        execFromCmd(argv[1]);
    } else {
        readEvalPrintLoop();
    }
    return 0;
}