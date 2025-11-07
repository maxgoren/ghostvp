#include <iostream>
#include "lexer.hpp"
#include "parser.hpp"
#include "prettyprint.hpp"
#include "interpreter.hpp"
using namespace std;

void readEvalPrintLoop(bool loud) {
    bool running = true;
    bool trace = loud;
    Lexer lexer;
    Parser parser;
    PrettyPrinter* pp = new PrettyPrinter(trace);
    ScopeResolver* sr = new ScopeResolver(trace);
    Interpreter* terp = new Interpreter();
    while (running) {
        cout<<"mgcgs> ";
        string input;
        getline(cin, input);
        if (input == ".quit") {
            running = false;
        } else if (input == ".trace") {
            trace = !trace;
        } else {
        auto ast = parser.parse(lexer.tokenizeInput(const_cast<char*>(input.data())), trace);
            sr->visit(ast);
            if (trace)
                pp->visit(ast);
            terp->visit(ast);
        }
    }
}

void execFromCmd(char* data) {
    Lexer lexer;
    Parser pp(lexer.tokenizeInput(data), true);
    auto t = pp.parseStmtList();
    PrettyPrinter* pv = new PrettyPrinter();
    pv->visit(t);
    ScopeResolver* sr = new ScopeResolver();
    sr->visit(t);
    Interpreter* ev = new Interpreter();
    ev->visit(t);
}

int main(int argc, char* argv[]) {
    if (argc > 1) {
        execFromCmd(argv[1]);
    } else {
        readEvalPrintLoop(false);
    }
    return 0;
}