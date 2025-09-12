#ifndef prettyprint_hpp
#define prettyprint_hpp
#include "ast.hpp"
#include <iostream>
using namespace std;

class PrettyPrinter : public Visitor {
    private:
        int depth;
        void enter() {
            depth++;
        }
        void leave() {
            depth--;
        }
        void say(string s) {
            for (int i = 0; i < depth; i++) cout<<" ";
            cout<<s<<endl;
        }
    public:
        PrettyPrinter() {
            depth = 0;
        }
        void visit(StatementList* sl) {
            for (auto s : sl->getList()) {
                s->accept(this);
            }
        }
        void visit(BlockStmt* stmt) {
            enter();
            say("Open Block Scope");
            stmt->getStatements()->accept(this);
            say("Closing blocking scope.");
            leave();
        }
        void visit(PrintStmt* stmt) {
            enter();
            say("Print");
            stmt->getExpr()->accept(this);
            leave();
        }
        void visit(WhileStmt* stmt) {
            enter();
            say("While Loop");
            stmt->getPredicate()->accept(this);
            stmt->getBody()->accept(this);
            leave();
        }
        void visit(IfStmt* stmt) {
            enter();
            say("If Stmt");
            say("Predicate: ");
            stmt->getPredicate()->accept(this);
            say("True Path: ");
            stmt->getTruePath()->accept(this);
            if (stmt->getFalsePath() != nullptr) {
                say("False Path: ");
                stmt->getFalsePath()->accept(this);
            }
            leave();
        }
        void visit(FuncDefStmt* stmt) {
            enter();
            say("Function Definition");
            stmt->getName()->accept(this);
            stmt->getParams()->accept(this);
            stmt->getBody()->accept(this);
            leave();
        }
        void visit(ReturnStmt* stmt) { 
            enter();
            say("Return Statement");
            stmt->getExpr()->accept(this);
            leave();
        }
        void visit(LetStmt* stmt) {
            enter();
            say("Let Statement");
            stmt->getExpr()->accept(this);
            leave();
        }
        void visit(ExprStmt* stmt) {
            enter();
            say("Expr Stmt");
            stmt->getExpr()->accept(this);
            leave();
        } 
        void visit(ExpressionList* exprList) {
            enter();
            for (auto expr : exprList->getExpressions()) {
                expr->accept(this);
            }
            leave();
        }
        void visit(SubscriptExpr* expr) {
            enter();
            say("Subscript: ");
            expr->getName()->accept(this);
            expr->getSubscript()->accept(this);
            leave();
        }
        void visit(ConstExpr* expr) {
            enter();
            say("ConstExpr " + expr->getToken().getString());
            leave();
        }
        void visit(IdExpr* expr) {
            enter();
            say("IdExpr " + expr->getToken().getString() + ", scope: " + to_string(expr->getToken().scopeLevel()));
            leave();
        }
        void visit(BinaryOpExpr* expr) {
            enter();
            say("Binary Op " + expr->getToken().getString());
            expr->getLeft()->accept(this);
            expr->getRight()->accept(this);
            leave();
        }
        void visit(UnaryOpExpr* expr) {
            enter();
            say("Unary Op " + expr->getToken().getString());
            expr->getExpr()->accept(this);
            leave();
        }
        void visit(FunctionCallExpr* expr) {
            enter();
            say("Function call");
            expr->getName()->accept(this);
            expr->getArguments()->accept(this);
            leave();
        }
        void visit(ArrayConstructorExpr* expr) {
            enter();
            say("Array Constructor");
            for (auto t : expr->getExpressions()) {
                t->accept(this);
            }
            leave();
        }
        void visit(LambdaExpr* expr) {
            enter();
            say("(lambda)");
            expr->getParams()->accept(this);
            expr->getBody()->accept(this);
            leave();
        }
};

#endif