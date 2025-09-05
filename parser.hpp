#ifndef parser_hpp
#define parser_hpp
#include <iostream>
#include <stack>
#include <vector>
#include "ast.hpp"
using namespace std;


class Parser {
    private:
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
                case TK_ASSIGN: return 20;
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
                case TK_LB: return 80;
                case TK_LP: return 100;
                default:
                    break;
            }
            return 10;
        }
        ExprNode* parseFirst(int prec) {
            ExprNode* node = nullptr;
            cout<<"Parse First on "<<current().getString()<<endl;
            switch (current().getSymbol()) {
                case TK_NUMBER:
                    node = new ConstExpr(current());
                    match(TK_NUMBER);
                    return node;
                case TK_STRING:
                    node = new ConstExpr(current());
                    match(TK_STRING);
                    return node;
                case TK_ID:
                    node = new IdExpr(current());
                    match(TK_ID);
                    return node;
                case TK_SUB: {
                    UnaryOpExpr* node = new UnaryOpExpr(current());
                    match(TK_SUB);
                    node->setExpr(parseExpression(precedence(TK_SUB)+40));
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
                    return node;
                } break;
                case TK_LP: {
                    match(TK_LP);
                    node = parseExpression(0);
                    match(TK_RP);
                } break;
                default:
                    break;
            }
            return node;
        }
        ExprNode* parseRest(ExprNode* lhs) {
            ExprNode* node = nullptr;
            cout<<"Parse Rest on: "<<current().getString()<<endl;
            switch (current().getSymbol()) {
                case TK_EQ:
                case TK_LT: 
                case TK_GT:
                case TK_LTE:
                case TK_GTE:
                case TK_NEQ:
                case TK_ASSIGN:
                case TK_ADD: 
                case TK_SUB:
                case TK_MUL:
                case TK_DIV: {
                    BinaryOpExpr* node = new BinaryOpExpr(current());
                    node->setLeft(lhs);
                    int p = precedence(current().getSymbol());
                    advance();
                    node->setRight(parseExpression(p+10));
                    return node;
                } break; 
                case TK_LP: {
                    FunctionCallExpr* node = new FunctionCallExpr(current());
                    node->setName((IdExpr*)lhs);
                    match(TK_LP);
                    node->setArguments(parseExprList());
                    match(TK_RP);
                    return node;
                } break;
                case TK_LB: {
                    SubscriptExpr* node = new SubscriptExpr(current());
                    node->setName((IdExpr*)lhs);
                    match(TK_LB);
                    node->setSubscript(parseExpression(0));
                    match(TK_RB);
                    return node;
                } break;
                case TK_RB: return lhs;
                case TK_RP: return lhs;
                default:
                    break;
            }
            return node == nullptr ? lhs:node;
        }
        ExprNode* parseExpression(int prec) {
            cout<<"ParseExpression on "<<current().getString()<<endl;
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
            cout<<"Parse Print Statement"<<endl;
            PrintStmt* node = new PrintStmt(current());
            match(TK_PRINTLN);
            node->setExpr(parseExpression(0));
            return node;
        }
        LetStmt* parseLet() {
            cout<<"Parse Let Statement"<<endl;
            LetStmt* node = new LetStmt(current());
            match(TK_LET);
            node->setExpression(parseExpression(0));
            return node;
        }
        WhileStmt* parseWhile() {
            cout<<"Parse While Statement"<<endl;
            WhileStmt* node = new WhileStmt(current());
            match(TK_WHILE);
            match(TK_LP);
            node->setPredicate(parseExpression(0));
            match(TK_RP);
            match(TK_LC);
            node->setBody(parseStmtList());
            match(TK_RC);
            return node;
        }
        IfStmt* parseIf() {
            cout<<"Parse If Statement"<<endl;
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
            FuncDefStmt* node = new FuncDefStmt(current());
            advance();
            node->setName((IdExpr*)parseFirst(0));
            match(TK_LP);
            node->setParams(parseParamList());
            match(TK_RP);
            match(TK_LC);
            node->setBody(parseStmtList());
            match(TK_RC);
            return node;
        }
        ReturnStmt* parseReturn() {
            ReturnStmt* node = new ReturnStmt(current());
            advance();
            node->setExpression(parseExpression(0));
            return node;
        }
        ExprStmt* parseExprStmt() {
            ExprStmt* node = new ExprStmt(current());
            node->setExpression(parseExpression(0));
            return node;
        }
        StmtNode* parseStmt() {
            Token curr = current();
            cout<<"Parse Statement on: "<<curr.getString()<<endl;
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
        }
    public:
        Parser(vector<Token> tkns) {
            init(tkns);
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
            return parseStmtList();
        }
};

#endif