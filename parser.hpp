#ifndef parser_hpp
#define parser_hpp
#include <iostream>
#include <stack>
#include <vector>
#include "ast.hpp"
#include "util.hpp"
using namespace std;


class Parser {
    private:
        DepthTracker dt;
        Token* tokens;
        int num_tokens;
        int token_pos;
        bool expect(TKSymbol symbol) {
            return symbol == tokens[token_pos].getSymbol();
        }
        bool match(TKSymbol symbol) {
            if (expect(symbol)) {
                advance();
                return true;
            }
            cout<<"Mismatched Token: "<<current().getString()<<endl;
            return false;
        }
        void advance() {
            if (tokens[token_pos].getSymbol() != TK_EOI) {
                token_pos++;
            }
        }
        Token current() {
            return tokens[token_pos];
        }
        Token previous() {
            return token_pos > 0 ? tokens[token_pos-1]:Token();
        }
        bool isSeperator(TKSymbol symbol) {
            switch (symbol) {
                case TK_LC:
                case TK_RC:
                case TK_SEMI:
                case TK_COMMA: return true;
                default:
                    break;
            }
            return false;
        }
        int precedence(TKSymbol oper) {
            switch (oper) {
                case TK_LAMBDA: return 10;
                case TK_ASSIGN: return 20;
                case TK_AND: return 22;
                case TK_OR: return 22;
                case TK_LT: return 25;
                case TK_GT: return 25;
                case TK_EQ: return 25;
                case TK_NEQ: return 25;
                case TK_LTE: return 25;
                case TK_GTE: return 25;
                case TK_SUB: return 50;
                case TK_ADD: return 50;
                case TK_MUL: return 60;
                case TK_DIV: return 60;
                case TK_MOD: return 60;
                case TK_LB: return 80;
                case TK_MAP:
                case TK_FILTER:
                case TK_REDUCE:
                case TK_PUSH:
                case TK_APPEND:
                case TK_FIRST:
                case TK_REST:
                case TK_GET:
                case TK_POP:
                case TK_EMPTY:
                case TK_SIZE:
                case TK_LP: return 100;
                default:
                    break;
            }
            return 10;
        }
        ExprNode* parseFirst(int prec) {
            ExprNode* node = nullptr;
            dt.enter("Parse First on " + current().getString());
            switch (current().getSymbol()) {
                case TK_NUMBER:
                    node = new ConstExpr(current());
                    match(TK_NUMBER);
                    dt.leave();
                    return node;
                case TK_STRING:
                    node = new ConstExpr(current());
                    match(TK_STRING);
                    dt.leave();
                    return node;
                case TK_ID:
                    node = new IdExpr(current());
                    match(TK_ID);
                    dt.leave();
                    return node;
                case TK_SUB: {
                    UnaryOpExpr* node = new UnaryOpExpr(current());
                    match(TK_SUB);
                    node->setExpr(parseExpression(precedence(TK_SUB)+40));
                    dt.leave();
                    return node;
                }  break;
                case TK_LB: {
                    ArrayConstructorExpr* node = new ArrayConstructorExpr(current());
                    match(TK_LB);
                    while (!expect(TK_EOI) && !expect(TK_RB)) {
                        if (expect(TK_COMMA))
                            match(TK_COMMA);
                        node->addExpr(parseExpression(0));
                    }
                    match(TK_RB);
                    dt.leave();
                    return node;
                } break;
                case TK_LP: {
                    match(TK_LP);
                    node = parseExpression(0);
                    match(TK_RP);
                    dt.leave();
                    return node;
                } break;
                case TK_LAMBDA: {
                    LambdaExpr* node = new LambdaExpr(current());
                    match(TK_LAMBDA);
                    node->setParams(parseParamList());
                    match(TK_RP);
                    if (expect(TK_PRODUCE)) {
                        match(TK_PRODUCE);
                        node->setBody(parseStmtList());
                    } else {
                        match(TK_LC);
                        node->setBody(parseStmtList());
                        match(TK_RC);
                    }
                    dt.leave();
                    return node;
                    break;
                }
                case TK_FIRST:
                case TK_REST:
                case TK_POP:
                case TK_EMPTY:
                case TK_SIZE: {
                    ListOpExpr* node = new ListOpExpr(current());
                    match(current().getSymbol());
                    match(TK_LP);
                    node->setList(parseExpression(0));
                    match(TK_RP);
                    return node;
                } break;
                case TK_MAP:
                case TK_FILTER:
                case TK_REDUCE:
                case TK_PUSH:
                case TK_APPEND:
                case TK_GET: {
                    ListOpExpr* node = new ListOpExpr(current());
                    match(current().getSymbol());
                    match(TK_LP);
                    node->setList(parseExpression(0));
                    match(TK_COMMA);
                    node->setExpr(parseExpression(0));
                    match(TK_RP);
                    return node;
                } break;
                default:
                    break;
            }
            return node;
        }
        ExprNode* parseRest(ExprNode* lhs) {
            ExprNode* node = nullptr;
            dt.enter("Parse rest on " + current().getSymbol());
            switch (current().getSymbol()) {
                case TK_EQ:
                case TK_LT: 
                case TK_GT:
                case TK_LTE:
                case TK_GTE:
                case TK_NEQ:
                case TK_AND:
                case TK_OR:
                case TK_ASSIGN:
                case TK_ADD: 
                case TK_SUB:
                case TK_MUL:
                case TK_DIV:
                case TK_MOD: {
                    BinaryOpExpr* node = new BinaryOpExpr(current());
                    node->setLeft(lhs);
                    int p = precedence(current().getSymbol());
                    advance();
                    node->setRight(parseExpression(p+10));
                    dt.leave();
                    return node;
                } break; 
                case TK_LP: {
                    FunctionCallExpr* node = new FunctionCallExpr(current());
                    node->setName((IdExpr*)lhs);
                    match(TK_LP);
                    node->setArguments(parseExprList());
                    match(TK_RP);
                    dt.leave();
                    return node;
                } break;
                case TK_LB: {
                    SubscriptExpr* node = new SubscriptExpr(current());
                    node->setName((IdExpr*)lhs);
                    match(TK_LB);
                    node->setSubscript(parseExpression(0));
                    match(TK_RB);
                    dt.leave();
                    return node;
                } break;
                case TK_RB:
                case TK_RP: 
                    dt.leave();
                    return lhs;
                default:
                    break;
            }
            dt.leave();
            return node;
            return node == nullptr ? lhs:node;
        }
        ExprNode* parseExpression(int prec) {
            dt.enter("ParseExpression on " + current().getString());
            ExprNode* node = parseFirst(prec);
            while (!expect(TK_EOI) && !isSeperator(current().getSymbol()) && prec <= precedence(current().getSymbol())) {
                if (expect(TK_RP) || expect(TK_RB))
                    return node;
                node = parseRest(node);
            }
            return node;
        }
        ExpressionList* parseExprList() {
            ExpressionList* el = new ExpressionList(current());
            while (!expect(TK_RP)) {
                if (expect(TK_COMMA)) advance();
                el->addExpr(parseExpression(0));
            }
            return el;
        }
        PrintStmt* parsePrint() {
            dt.enter("Parse Print Statement");
            PrintStmt* node = new PrintStmt(current());
            match(TK_PRINTLN);
            node->setExpr(parseExpression(0));
            dt.leave();
            return node;
        }
        LetStmt* parseLet() {
            dt.enter("Parse Let Statement");
            LetStmt* node = new LetStmt(current());
            if (expect(TK_LET))
                match(TK_LET);
            node->setExpression(parseExpression(0));
            dt.leave();
            return node;
        }
        WhileStmt* parseWhile() {
            dt.enter("Parse While Statement");
            WhileStmt* node = new WhileStmt(current());
            match(TK_WHILE);
            match(TK_LP);
            node->setPredicate(parseExpression(0));
            match(TK_RP);
            match(TK_LC);
            node->setBody(parseStmtList());
            match(TK_RC);
            dt.leave();
            return node;
        }
        IfStmt* parseIf() {
            dt.enter("Parse If Statement");
            IfStmt* node = new IfStmt(current());
            match(TK_IF);
            match(TK_LP);
            node->setPredicate(parseExpression(0));
            match(TK_RP);
            match(TK_LC);
            node->setTruePath(parseStmtList());
            match(TK_RC);
            if (expect(TK_ELSE)) {
                match(TK_ELSE);
                match(TK_LC);
                node->setFalsePath(parseStmtList());
                match(TK_RC);
            } else {
                node->setFalsePath(nullptr);
            }
            dt.leave();
            return node;
        }
        StatementList* parseParamList() {
            StatementList* sl = new StatementList(current());
            while (!expect(TK_EOI) && !expect(TK_RP)) {
                sl->addStatement(parseLet());
                if (expect(TK_COMMA)) {
                    match(TK_COMMA);
                }
            }
            return sl;
        }
        FuncDefStmt* parseFunctionDefinition() {
            dt.enter("Parse Function Definition");
            FuncDefStmt* node = new FuncDefStmt(current());
            advance();
            node->setName((IdExpr*)parseFirst(0));
            match(TK_LP);
            node->setParams(parseParamList());
            match(TK_RP);
            match(TK_LC);
            node->setBody(parseStmtList());
            match(TK_RC);
            dt.leave();
            return node;
        }
        ReturnStmt* parseReturn() {
            dt.enter("Parse Return statement");
            ReturnStmt* node = new ReturnStmt(current());
            advance();
            node->setExpression(parseExpression(0));
            dt.leave();
            return node;
        }
        ExprStmt* parseExprStmt() {
            dt.enter("Parse Expr Statement");
            ExprStmt* node = new ExprStmt(current());
            node->setExpression(parseExpression(0));
            dt.leave();
            return node;
        }
        StmtNode* parseStmt() {
            Token curr = current();
            dt.say("Parse Statement on " + curr.getString());
            if (expect(TK_PRINTLN)) {
                return parsePrint();
            } else if (expect(TK_LET)) {
                return parseLet();
            } else if (expect(TK_WHILE)) {
                return parseWhile();
            } else if (expect(TK_IF)) {
                return parseIf();
            } else if (expect(TK_DEF)) {
                return parseFunctionDefinition();
            } else if (expect(TK_RETURN)) {   
                return parseReturn();
            } else if (expect(TK_LC)) {
                BlockStmt* bs = new BlockStmt(current());
                match(TK_LC);
                bs->setStatements(parseStmtList());
                match(TK_RC);
                return bs;
            } else {
                return parseExprStmt();
            }
            return nullptr;
        }
        void init(vector<Token>& tkns) {
            int n = tkns.size();
            tokens = new Token[n];
            for (int i = 0; i < n; i++) {
                tokens[i] = tkns[i];
            }
            num_tokens = n;
            token_pos = 0;
            dt.reset();
        }
    public:
        Parser(vector<Token> tkns) {
            init(tkns);
            dt.reset();
        }
        Parser() { }
        StatementList* parseStmtList() {
            StatementList* sl = new StatementList(current());
            while (!expect(TK_EOI)) {
                if (expect(TK_RC)) {
                    return sl;
                }
                sl->addStatement(parseStmt());
                if (expect(TK_SEMI)) advance();
            }
            return sl;
        }
        StatementList* parse(vector<Token> tkns) {
            init(tkns);
            StatementList* sl = parseStmtList();
            cout<<"Parse Complete."<<endl;
            return sl;
        }
};

#endif