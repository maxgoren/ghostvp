#ifndef resolvescope_hpp
#define resolvescope_hpp
#include <iostream>
#include <unordered_map>
#include <vector>
#include "ast.hpp"
#include "util.hpp"
#include "stack.hpp"
using namespace std;

class ScopeResolver : public Visitor {
    private:
    bool loud;
        DepthTracker dt;
        InsepctableStack<unordered_map<string, bool>> defs;
        void openScope() {
            dt.say("Opening Scope");
            defs.push(unordered_map<string,bool>());
        }
        void closeScope() {
            defs.pop();
            dt.say("Scope closed.");
        }
        void declareVarName(string name) {
            if (defs.empty()) {
                dt.say("In global scope");
                return;
            }
            if (defs.top().find(name) != defs.top().end()) {
                dt.say(name + " already defined");
                return;
            }
            defs.top()[name] = false;
        }
        void defineVarName(string name) {
            if (defs.empty()) {
                dt.say("In global scope.");
                return;
            }
            defs.top()[name] = true;
        }
        void resolveVariableDepth(IdExpr* node, string name) {
            if (defs.empty()) {
                dt.say("In global scope");
                return;
            }
            for (int i = defs.size()-1; i >= 0; i--) {
                if (defs.get(i).find(name) != defs.get(i).end()) {
                    node->getToken().setScopeLevel(defs.size() - 1 - i);
                    dt.say(name + " resolved at scope " + to_string(node->getToken().scopeLevel()));
                    return;
                }
            }
            dt.say("Nah man, couldnt find " + name);
        }
    public:
        ScopeResolver(bool trace = false) {
            loud = trace;
            dt = DepthTracker(loud);
        }
        void visit(IdExpr* expr) {
            dt.enter();
            dt.say("Resolving Id expression for " + expr->getToken().getString());
            resolveVariableDepth(expr, expr->getToken().getString());
            dt.leave();
        }
        void visit(FunctionCallExpr* expr) {
            dt.enter();
            dt.say("Resolving function call from " + expr->getName()->getToken().getString());
            //resolveVariableDepth(expr->getName(), expr->getName()->getToken().getString());
            expr->getName()->accept(this);
            expr->getArguments()->accept(this);
            dt.leave();
        }
        void visit(BinaryOpExpr* expr) {
            dt.enter();
            dt.say("Resolving Binary Operator: " + expr->getToken().getString());
            expr->getLeft()->accept(this);
            expr->getRight()->accept(this);
            dt.leave();
        }
        void visit(FuncDefStmt* stmt) {
            dt.enter();
            string name = stmt->getName()->getToken().getString();
            dt.say("Resolving function definition " + name);
            declareVarName(name);
            defineVarName(name);
            stmt->getName()->accept(this); 
            openScope();
            stmt->getParams()->accept(this);
            stmt->getBody()->accept(this);
            closeScope();
            dt.leave();
        }
        void visit(LambdaExpr* expr) {
            dt.enter("Resolving Lambda Expr");
            openScope();
            expr->getParams()->accept(this);
            expr->getBody()->accept(this);
            closeScope();
            dt.leave();
        }
        void visit(BlockStmt* stmt) {
            dt.enter("Resolving block stmt");
            openScope();
            stmt->getStatements()->accept(this);
            closeScope();
            dt.leave();
        }
        void visit(StatementList* stmt) {
            dt.enter("Resolving statement list");
            for (auto t : stmt->getList()) {
                t->accept(this);
            }
            dt.leave();
        }
        void visit(LetStmt* stmt) {
            dt.enter("Resolving Let stmt");
            if (stmt->getExpression()->getToken().getSymbol() == TK_ID) {
                declareVarName(stmt->getExpression()->getToken().getString());
                stmt->getExpression()->accept(this);
                defineVarName(stmt->getExpression()->getToken().getString());
            } else if (stmt->getExpression()->getToken().getSymbol() == TK_ASSIGN) {
                BinaryOpExpr* expr = (BinaryOpExpr*)stmt->getExpression();
                declareVarName(expr->getLeft()->getToken().getString());
                stmt->getExpression()->accept(this);
                declareVarName(expr->getLeft()->getToken().getString());
            }
            dt.leave();
        }
        void visit(PrintStmt* stmt) {
            dt.enter("Resolving Print stmt");
            stmt->getExpr()->accept(this); 
            dt.leave();
        }
        void visit(ExprStmt* stmt) { stmt->getExpression()->accept(this); }
        void visit(WhileStmt* stmt) {
            dt.enter("Resolving While stmt"); 
            stmt->getPredicate()->accept(this); 
            stmt->getBody()->accept(this);
            dt.leave(); 
        }
        void visit(IfStmt* stmt) { 
            dt.enter("Resolving If Statement");
            dt.say("Predicate: ");
            stmt->getPredicate()->accept(this);
            dt.say("True branch: ");
            stmt->getTruePath()->accept(this);
            if (stmt->getFalsePath() != nullptr) {
                dt.say("False branch: ");
                stmt->getFalsePath()->accept(this);
            }
            dt.leave();
        }
        void visit(ReturnStmt* stmt) { stmt->getExpression()->accept(this); }
        void visit(ExpressionList* expr) { 
            for (auto t : expr->getExpressions()) {
                t->accept(this);
            }
        }
        void visit(SubscriptExpr* expr) {
            expr->getName()->accept(this);
            expr->getSubsript()->accept(this);
        }
        void visit(ArrayConstructorExpr* expr) {
            for (auto t : expr->getExpressions()) {
                t->accept(this);
            }
         }
         void visit(ListOpExpr* expr) {
            dt.enter("Resolving ListOp " + expr->getToken().getString());
            expr->getList()->accept(this);
            if (expr->getExpr() != nullptr) expr->getExpr()->accept(this);
            dt.leave();
        }
         void visit(ConstExpr* expr) {

         }
         void visit(UnaryOpExpr* expr) {
            expr->getExpr()->accept(this);
         }
         void visit(ObjectConstructorExpr* expr) {
            dt.enter("Object Constructor");
            expr->getName()->accept(this);
            for (auto t : expr->getExpressions()) {
                t->accept(this);
            }
            dt.leave();
        }
        void visit(ObjectDefStmt* stmt) {
            dt.enter("Object def");
            string name = stmt->getName()->getToken().getString();
            dt.say("Resolving function definition " + name);
            declareVarName(name);
            defineVarName(name);
            stmt->getName()->accept(this);
            openScope();            
            stmt->getBody()->accept(this);
            closeScope();
            dt.leave();
        }
};

#endif