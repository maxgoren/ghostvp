<<<<<<< HEAD:prettyprint.hpp
#ifndef prettyprint_hpp
#define prettyprint_hpp
#include "ast.hpp"
#include "util.hpp"
#include <iostream>
using namespace std;

class PrettyPrinter : public Visitor {
    private:
        DepthTracker dt;
    public:
        PrettyPrinter() {
        }
        void visit(StatementList* sl) {
            for (auto s : sl->getList()) {
                s->accept(this);
            }
        }
        void visit(BlockStmt* stmt) {
            dt.enter();
            dt.say("Open block scope");
            stmt->getStatements()->accept(this);
            dt.leave();
        }
        void visit(PrintStmt* stmt) {
            dt.enter();
            dt.say("Print");
            stmt->getExpr()->accept(this);
            dt.leave();
        }
        void visit(WhileStmt* stmt) {
            dt.enter();
            dt.say("While Loop");
            stmt->getPredicate()->accept(this);
            stmt->getBody()->accept(this);
            dt.leave();
        }
        void visit(IfStmt* stmt) {
            dt.enter();
            dt.say("If Stmt");
            dt.say("Predicate: ");
            stmt->getPredicate()->accept(this);
            dt.say("True Path: ");
            stmt->getTruePath()->accept(this);
            if (stmt->getFalsePath() != nullptr) {
                dt.say("False Path: ");
                stmt->getFalsePath()->accept(this);
            }
            dt.leave();
        }
        void visit(FuncDefStmt* stmt) {
            dt.enter();
            dt.say("Function Definition");
            stmt->getName()->accept(this);
            stmt->getParams()->accept(this);
            stmt->getBody()->accept(this);
            dt.leave();
        }
        void visit(ReturnStmt* stmt) { 
            dt.enter();
            dt.say("Return Statement");
            stmt->getExpression()->accept(this);
            dt.leave();
        }
        void visit(LetStmt* stmt) {
            dt.enter();
            dt.say("Let Statement");
            stmt->getExpression()->accept(this);
            dt.leave();
        }
        void visit(ExprStmt* stmt) {
            dt.enter();
            dt.say("Expr Stmt");
            stmt->getExpression()->accept(this);
            dt.leave();
        } 
        void visit(LambdaExpr* expr) {
            dt.enter();
            dt.say("Lambda Expr");
            expr->getParams()->accept(this);
            expr->getBody()->accept(this);
            dt.leave();
        }
        void visit(ExpressionList* exprList) {
            dt.enter();
            for (auto expr : exprList->getExpressions()) {
                expr->accept(this);
            }
            dt.leave();
        }
        void visit(SubscriptExpr* expr) {
            dt.enter();
            dt.say("Subscript: ");
            expr->getName()->accept(this);
            expr->getSubsript()->accept(this);
            dt.leave();
        }
        void visit(ConstExpr* expr) {
            dt.enter();
            dt.say("ConstExpr " + expr->getToken().getString());
            dt.leave();
        }
        void visit(IdExpr* expr) {
            dt.enter();
            dt.say("IdExpr " + expr->getToken().getString() + ", " + to_string(expr->getToken().scopeLevel()));
            dt.leave();
        }
        void visit(BinaryOpExpr* expr) {
            dt.enter();
            dt.say("Binary Op " + expr->getToken().getString());
            expr->getLeft()->accept(this);
            expr->getRight()->accept(this);
            dt.leave();
        }
        void visit(UnaryOpExpr* expr) {
            dt.enter();
            dt.say("Unary Op " + expr->getToken().getString());
            expr->getExpr()->accept(this);
            dt.leave();
        }
        void visit(FunctionCallExpr* expr) {
            dt.enter();
            dt.say("Function call");
            expr->getName()->accept(this);
            expr->getArguments()->accept(this);
            dt.leave();
        }
        void visit(ArrayConstructorExpr* expr) {
            dt.enter();
            dt.say("Array Constructor");
            for (auto t : expr->getExpressions()) {
                t->accept(this);
            }
            dt.leave();
        }
        void visit(ListOpExpr* expr) {
            dt.enter("ListOp " + expr->getToken().getString());
            expr->getList()->accept(this);
            if (expr->getExpr() != nullptr) expr->getExpr()->accept(this);
            dt.leave();
        }
};

=======
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

>>>>>>> 86f9ac45c8927245ccea6bc7db5f04f076f4188b:src/prettyprint.hpp
#endif