#include <iostream>
#include "parse/lexer.hpp"
#include "parse/parser.hpp"
#include "interpreter/prettyprint.hpp"
#include "interpreter/interpreter.hpp"
using namespace std;

void readEvalPrintLoop(bool loud) {
    bool running = true;
    bool trace = loud;
    Lexer lexer;
    Parser parser;
    PrettyPrinter* pp = new PrettyPrinter(trace);
    ScopeResolver* sr = new ScopeResolver(trace);
    Interpreter* terp = new Interpreter();
    StringBuffer* sb = new StringBuffer();
    while (running) {
        cout<<"mgcgs> ";
        string input;
        getline(cin, input);
        if (input == ".quit") {
            running = false;
        } else if (input == ".trace") {
            trace = !trace;
        } else {
            sb->init(input);
        auto ast = parser.parse(lexer.tokenizeInput(sb), true);
            sr->visit(ast);
            if (trace)
                pp->visit(ast);
            terp->visit(ast);
        }
    }
}

void execFromCmd(CharBuffer* data) {
    Lexer lexer;
    Parser pp;
    auto t = pp.parse(lexer.tokenizeInput(data), true);
    PrettyPrinter* pv = new PrettyPrinter();
    pv->visit(t);
    ScopeResolver* sr = new ScopeResolver();
    sr->visit(t);
    Interpreter* ev = new Interpreter();
    ev->visit(t);
}

int main(int argc, char* argv[]) {
    if (argc > 1) {
        CharBuffer* buff;
        if (argv[1][0] == '-' && argv[1][1] == 'f') {
            FileStringBuffer* fsb = new FileStringBuffer();
            fsb->readFile(argv[2]);
            buff = fsb;
        } else {
            StringBuffer* sb = new StringBuffer();
            sb->init(argv[1]);
        }
        execFromCmd(buff);
    } else {
        readEvalPrintLoop(true);
    }
    return 0;
}