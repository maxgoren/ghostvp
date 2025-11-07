<<<<<<< HEAD:ast.hpp
#ifndef ast_hpp
#define ast_hpp
#include <list>
#include <iostream>
#include <unordered_map>
#include "token.hpp"
using namespace std;

class BlockStmt;
class StatementList;
class ExprStmt;
class PrintStmt;
class LetStmt;
class WhileStmt;
class IfStmt;
class FuncDefStmt;
class ReturnStmt;

class ExpressionList;
class UnaryOpExpr;
class BinaryOpExpr;
class ConstExpr;
class IdExpr;
class FunctionCallExpr;
class SubscriptExpr;
class ArrayConstructorExpr;
class ListOpExpr;
class LambdaExpr;

class Visitor {
    public:
        //statement node visitors
        virtual void visit(BlockStmt* stmt) = 0;
        virtual void visit(StatementList* stmtList) = 0;
        virtual void visit(ExprStmt* stmt) = 0;
        virtual void visit(PrintStmt* stmt) = 0;
        virtual void visit(LetStmt* stmt) = 0;
        virtual void visit(WhileStmt* stmt) = 0;
        virtual void visit(IfStmt* stmt) = 0;
        virtual void visit(FuncDefStmt* stmt) = 0;
        virtual void visit(ReturnStmt* stmt) = 0;
        //expression node visitors
        virtual void visit(ExpressionList* expr) = 0;
        virtual void visit(UnaryOpExpr* expr) = 0;
        virtual void visit(BinaryOpExpr* expr) = 0;
        virtual void visit(ConstExpr* expr) = 0;
        virtual void visit(IdExpr* expr) = 0;
        virtual void visit(FunctionCallExpr* expr) = 0;
        virtual void visit(SubscriptExpr* expr) = 0;
        virtual void visit(ArrayConstructorExpr* expr) = 0;
        virtual void visit(ListOpExpr* expr) = 0;
        virtual void visit(LambdaExpr* expr) = 0;
};

class SyntaxNode {
    private:
        Token token;
    public:
        SyntaxNode(Token t) : token(t) { }
        Token& getToken() {
            return token;
        }
        virtual void accept(Visitor* visit) = 0;
};

class ExprNode : public SyntaxNode {
    public:
        ExprNode(Token tk) : SyntaxNode(tk) { }
};

class StmtNode : public SyntaxNode {
    public:
        StmtNode(Token tk) : SyntaxNode(tk) { }
};

class StatementList : public StmtNode {
    private:
        list<StmtNode*> statements;
    public:
        StatementList(Token tk) : StmtNode(tk) { }
        ~StatementList() {
            for (auto t : statements) {
                delete t;
            }
        }
        list<StmtNode*>& getList() {
            return statements;
        }
        void addStatement(StmtNode* stmt) {
            statements.push_back(stmt);
        }
        void accept(Visitor* visitor) {
            visitor->visit(this);
        }
};

class BlockStmt : public StmtNode {
    private:
        StatementList* statements;
    public:
        BlockStmt(Token tk) : StmtNode(tk) { }
        ~BlockStmt() { delete statements; }
        StatementList* getStatements() {
            return statements;
        }
        void setStatements(StatementList* stmt) {
            statements = stmt;
        }
        void accept(Visitor* visit) {
            visit->visit(this);
        }
};

class PrintStmt : public StmtNode {
    private:
        ExprNode* expression;
    public:
        PrintStmt(Token tk) : StmtNode(tk) { }
        ~PrintStmt() { delete expression; }
        void setExpr(ExprNode* expr) {
            expression = expr;
        }
        ExprNode* getExpr() {
            return expression;
        }
        void accept(Visitor* visitor) {
            visitor->visit(this);
        }
};

class ExprStmt : public StmtNode {
    private:
        ExprNode* expression;
    public:
        ExprStmt(Token tk) : StmtNode(tk) { }
        ~ExprStmt() {
            delete expression;
        }
        ExprNode* getExpression() {
            return expression;
        }
        void setExpression(ExprNode* expr) {
            expression = expr;
        }
        void accept(Visitor* visitor) {
            visitor->visit(this);
        }
};

class LetStmt : public StmtNode {
    private:
        ExprNode* expression;
    public:
        LetStmt(Token tk) : StmtNode(tk) { }
        ~LetStmt() {
            delete expression;
        }
        ExprNode* getExpression() {
            return expression;
        }
        void setExpression(ExprNode* expr) {
            expression = expr;
        }
        void accept(Visitor* visitor) {
            visitor->visit(this);
        }
};

class WhileStmt : public StmtNode {
    private:
        ExprNode* predicate;
        StatementList* loopBody;
    public:
        WhileStmt(Token tk) : StmtNode(tk) { }
        ~WhileStmt() { 
            delete predicate; 
            delete loopBody;
        }
        void setPredicate(ExprNode* expr) {
            predicate = expr;
        }
        void setBody(StatementList* sl) {
            loopBody = sl;
        }
        ExprNode* getPredicate() {
            return predicate;
        }
        StatementList* getBody() {
            return loopBody;
        }
        void accept(Visitor* visitor) {
            visitor->visit(this);
        }
};

class IfStmt : public StmtNode {
    private:
        ExprNode* predicate;
        StatementList* truePath;
        StatementList* falsePath;
    public:
        IfStmt(Token tk) : StmtNode(tk) { }
        ~IfStmt() {
            delete predicate;
            delete truePath;
            delete falsePath;
        }
        void setPredicate(ExprNode* expr) {
            predicate = expr;
        }
        void setTruePath(StatementList* sl) {
            truePath = sl;
        }
        void setFalsePath(StatementList* sl) {
            falsePath = sl;
        }
        ExprNode* getPredicate() {
            return predicate;
        }
        StatementList* getTruePath() {
            return truePath;
        }
        StatementList* getFalsePath() {
            return falsePath;
        }
        void accept(Visitor* visitor) {
            visitor->visit(this);
        }
};

class ReturnStmt : public StmtNode {
    private:
        ExprNode* expression;
    public:
        ReturnStmt(Token tk) : StmtNode(tk) { }
        ~ReturnStmt() { delete expression; }
        void setExpression(ExprNode* expr) {
            expression = expr;
        }
        ExprNode* getExpression() {
            return expression;
        }
        void accept(Visitor* visitor) {
            visitor->visit(this);
        }
};

class IdExpr : public ExprNode {
    public:
        IdExpr(Token tk) : ExprNode(tk) { }
        ~IdExpr() { }
        void accept(Visitor* visitor) {
            visitor->visit(this);
        }
};

class FuncDefStmt : public StmtNode {  
    private:
        IdExpr* name;
        StatementList* params;
        StatementList* body;
    public:
        FuncDefStmt(Token tk) : StmtNode(tk) { }
        ~FuncDefStmt() {
            delete name;
            delete params;
            delete body;
        }
        void accept(Visitor* visitor) {
            visitor->visit(this);
        }
        void setName(IdExpr* expr) {
            name = expr;
        }
        void setParams(StatementList* exprs) {
            params = exprs;
        }
        void setBody(StatementList* stmts) {
            body = stmts;
        }
        IdExpr* getName() {
            return name;
        }
        StatementList* getParams() {
            return params;
        }
        StatementList* getBody() {
            return body;
        }
};

class ExpressionList : public ExprNode {
    private:
        list<ExprNode*> exprs;
    public:
        ExpressionList(Token tk) : ExprNode(tk) { }
        ~ExpressionList() {
            for (auto t : exprs) {
                delete t;
            }
        }
        void addExpr(ExprNode* expr) {
            exprs.push_back(expr);
        }
        list<ExprNode*> getExpressions() {
            return exprs;
        }
        void accept(Visitor* visitor) {
            visitor->visit(this);
        }
};

class ArrayConstructorExpr : public ExprNode {
    private:
        list<ExprNode*> exprs;
    public:
        ArrayConstructorExpr(Token tk) : ExprNode(tk) { }
        list<ExprNode*>& getExpressions() {
            return exprs;
        }
        void addExpr(ExprNode* expr) {
            exprs.push_back(expr);
        }
        void accept(Visitor* visit) {
            visit->visit(this);
        }
};

class ConstExpr : public ExprNode {
    public:
        ConstExpr(Token tk) : ExprNode(tk) { }
        ~ConstExpr() { }
        void accept(Visitor* visitor) {
            visitor->visit(this);
        }
};

class SubscriptExpr : public ExprNode {
    private:
        IdExpr* name;
        ExprNode* subscript;
    public:
        SubscriptExpr(Token tk) : ExprNode(tk) { }
        ~SubscriptExpr() {
            delete name;
            delete subscript;
        }
        void setName(IdExpr* expr) {
            name = expr;
        }
        void setSubscript(ExprNode* expr) {
            subscript = expr;
        }
        IdExpr* getName() {
            return name;
        }
        ExprNode* getSubsript() {
            return subscript;
        }
        void accept(Visitor* visitor) {
            visitor->visit(this);
        }
};

class BinaryOpExpr : public ExprNode {
    private:
        ExprNode* leftChild;
        ExprNode* rightChild;
    public:
        BinaryOpExpr(Token tk) : ExprNode(tk) { }
        ~BinaryOpExpr() {
            delete leftChild;
            delete rightChild;
        }
        ExprNode* getLeft() {
            return leftChild;
        }
        ExprNode* getRight() {
            return rightChild;
        }
        void setLeft(ExprNode* lhs) {
            leftChild = lhs;
        }
        void setRight(ExprNode* rhs) {
            rightChild = rhs;
        }
        void accept(Visitor* visitor) {
            visitor->visit(this);
        }
};

class UnaryOpExpr : public ExprNode {
    private:
        ExprNode* expression;
    public:
        UnaryOpExpr(Token tk) : ExprNode(tk) { }
        ~UnaryOpExpr() {
            delete expression;
        }
        void setExpr(ExprNode* expr) {
            expression = expr;
        }
        ExprNode* getExpr() {
            return expression;
        }
        void accept(Visitor* visitor) {
            visitor->visit(this);
        }
};

class FunctionCallExpr : public ExprNode {
    private:
        IdExpr* name;
        ExpressionList* arguments;
    public:
        FunctionCallExpr(Token tk) : ExprNode(tk) { }
        ~FunctionCallExpr() {
            delete name;
            delete arguments;
        }
        void accept(Visitor* visitor) {
            visitor->visit(this);
        }
        void setName(IdExpr* expr) {
            name = expr;
        }
        IdExpr* getName() {
            return name;
        }
        ExpressionList* getArguments() {
            return arguments;
        }
        void setArguments(ExpressionList* exprs) {
            arguments = exprs;
        }
};

class LambdaExpr : public ExprNode {
        StatementList* params;
        StatementList* body;
    public:
        LambdaExpr(Token tk) : ExprNode(tk) { }
        ~LambdaExpr() {
            delete params;
            delete body;
        }
        void accept(Visitor* visitor) {
            visitor->visit(this);
        }
        void setParams(StatementList* exprs) {
            params = exprs;
        }
        void setBody(StatementList* stmts) {
            body = stmts;
        }
        StatementList* getParams() {
            return params;
        }
        StatementList* getBody() {
            return body;
        }
};

class ListOpExpr : public ExprNode {
    private:
        ExprNode* listExpr;
        ExprNode* expr;
    public:
        ListOpExpr(Token tk) : ExprNode(tk) { }
        ~ListOpExpr() { }
         void accept(Visitor* visitor) {
            visitor->visit(this);
        }
        void setList(ExprNode* expr) {
            listExpr = expr;
        }
        ExprNode* getList() {
            return listExpr;
        }
        void setExpr(ExprNode* exp) {
            expr= exp;
        }
        ExprNode* getExpr() {
            return expr;
        }
};



=======
#ifndef ast_hpp
#define ast_hpp
#include <list>
#include <iostream>
#include <unordered_map>
#include "token.hpp"
using namespace std;

class BlockStmt;
class StatementList;
class ExprStmt;
class PrintStmt;
class LetStmt;
class WhileStmt;
class IfStmt;
class FuncDefStmt;
class ReturnStmt;

class ExpressionList;
class UnaryOpExpr;
class BinaryOpExpr;
class ConstExpr;
class IdExpr;
class FunctionCallExpr;
class SubscriptExpr;
class ArrayConstructorExpr;
class LambdaExpr;

class Visitor {
    public:
        //statement node visitors
        virtual void visit(BlockStmt* stmt) = 0;
        virtual void visit(StatementList* stmtList) = 0;
        virtual void visit(ExprStmt* stmt) = 0;
        virtual void visit(PrintStmt* stmt) = 0;
        virtual void visit(LetStmt* stmt) = 0;
        virtual void visit(WhileStmt* stmt) = 0;
        virtual void visit(IfStmt* stmt) = 0;
        virtual void visit(FuncDefStmt* stmt) = 0;
        virtual void visit(ReturnStmt* stmt) = 0;
        //expression node visitors
        virtual void visit(ExpressionList* expr) = 0;
        virtual void visit(UnaryOpExpr* expr) = 0;
        virtual void visit(BinaryOpExpr* expr) = 0;
        virtual void visit(ConstExpr* expr) = 0;
        virtual void visit(IdExpr* expr) = 0;
        virtual void visit(FunctionCallExpr* expr) = 0;
        virtual void visit(SubscriptExpr* expr) = 0;
        virtual void visit(ArrayConstructorExpr* expr) = 0;
        virtual void visit(LambdaExpr* expr) = 0;
};

class SyntaxNode {
    private:
        Token token;
    public:
        SyntaxNode(Token t) : token(t) { }
        Token& getToken() {
            return token;
        }
        virtual void accept(Visitor* visit) = 0;
};

class ExprNode : public SyntaxNode {
    public:
        ExprNode(Token tk) : SyntaxNode(tk) { }
};

class StmtNode : public SyntaxNode {
    public:
        StmtNode(Token tk) : SyntaxNode(tk) { }
};

class StatementList : public StmtNode {
    private:
        list<StmtNode*> statements;
    public:
        StatementList(Token tk) : StmtNode(tk) { }
        ~StatementList() {
            for (auto t : statements) {
                delete t;
            }
        }
        list<StmtNode*>& getList() {
            return statements;
        }
        void addStatement(StmtNode* stmt) {
            statements.push_back(stmt);
        }
        void accept(Visitor* visitor) {
            visitor->visit(this);
        }
};

class BlockStmt : public StmtNode {
    private:
        StatementList* statements;
    public:
        BlockStmt(Token tk) : StmtNode(tk) { }
        void accept(Visitor* visitor) {
            visitor->visit(this);
        }
        void setStatements(StatementList* stmt) {
            statements = stmt;
        }
        StatementList* getStatements() {
            return statements;
        }
};

class PrintStmt : public StmtNode {
    private:
        ExprNode* expression;
    public:
        PrintStmt(Token tk) : StmtNode(tk) { }
        ~PrintStmt() { delete expression; }
        void setExpr(ExprNode* expr) {
            expression = expr;
        }
        ExprNode* getExpr() {
            return expression;
        }
        void accept(Visitor* visitor) {
            visitor->visit(this);
        }
};

class ExprStmt : public StmtNode {
    private:
        ExprNode* expression;
    public:
        ExprStmt(Token tk) : StmtNode(tk) { }
        ~ExprStmt() {
            delete expression;
        }
        ExprNode* getExpr() {
            return expression;
        }
        void setExpr(ExprNode* expr) {
            expression = expr;
        }
        void accept(Visitor* visitor) {
            visitor->visit(this);
        }
};

class LetStmt : public StmtNode {
    private:
        ExprNode* expression;
    public:
        LetStmt(Token tk) : StmtNode(tk) { }
        ~LetStmt() {
            delete expression;
        }
        ExprNode* getExpr() {
            return expression;
        }
        void setExpr(ExprNode* expr) {
            expression = expr;
        }
        void accept(Visitor* visitor) {
            visitor->visit(this);
        }
};

class WhileStmt : public StmtNode {
    private:
        ExprNode* predicate;
        StatementList* loopBody;
    public:
        WhileStmt(Token tk) : StmtNode(tk) { }
        ~WhileStmt() { 
            delete predicate; 
            delete loopBody;
        }
        void setPredicate(ExprNode* expr) {
            predicate = expr;
        }
        void setBody(StatementList* sl) {
            loopBody = sl;
        }
        ExprNode* getPredicate() {
            return predicate;
        }
        StatementList* getBody() {
            return loopBody;
        }
        void accept(Visitor* visitor) {
            visitor->visit(this);
        }
};

class IfStmt : public StmtNode {
    private:
        ExprNode* predicate;
        StatementList* truePath;
        StatementList* falsePath;
    public:
        IfStmt(Token tk) : StmtNode(tk) { }
        ~IfStmt() {
            delete predicate;
            delete truePath;
            delete falsePath;
        }
        void setPredicate(ExprNode* expr) {
            predicate = expr;
        }
        void setTruePath(StatementList* sl) {
            truePath = sl;
        }
        void setFalsePath(StatementList* sl) {
            falsePath = sl;
        }
        ExprNode* getPredicate() {
            return predicate;
        }
        StatementList* getTruePath() {
            return truePath;
        }
        StatementList* getFalsePath() {
            return falsePath;
        }
        void accept(Visitor* visitor) {
            visitor->visit(this);
        }
};

class ReturnStmt : public StmtNode {
    private:
        ExprNode* expression;
    public:
        ReturnStmt(Token tk) : StmtNode(tk) { }
        ~ReturnStmt() { delete expression; }
        void setExpression(ExprNode* expr) {
            expression = expr;
        }
        ExprNode* getExpr() {
            return expression;
        }
        void accept(Visitor* visitor) {
            visitor->visit(this);
        }
};

class IdExpr : public ExprNode {
    public:
        IdExpr(Token tk) : ExprNode(tk) { }
        ~IdExpr() { }
        void accept(Visitor* visitor) {
            visitor->visit(this);
        }
};

class FuncDefStmt : public StmtNode {  
    private:
        IdExpr* name;
        StatementList* params;
        StatementList* body;
    public:
        FuncDefStmt(Token tk) : StmtNode(tk) { }
        ~FuncDefStmt() {
            delete name;
            delete params;
            delete body;
        }
        void accept(Visitor* visitor) {
            visitor->visit(this);
        }
        void setName(IdExpr* expr) {
            name = expr;
        }
        void setParams(StatementList* exprs) {
            params = exprs;
        }
        void setBody(StatementList* stmts) {
            body = stmts;
        }
        IdExpr* getName() {
            return name;
        }
        StatementList* getParams() {
            return params;
        }
        StatementList* getBody() {
            return body;
        }
};

class ExpressionList : public ExprNode {
    private:
        list<ExprNode*> exprs;
    public:
        ExpressionList(Token tk) : ExprNode(tk) { }
        ~ExpressionList() {
            for (auto t : exprs) {
                delete t;
            }
        }
        void addExpr(ExprNode* expr) {
            exprs.push_back(expr);
        }
        list<ExprNode*> getExpressions() {
            return exprs;
        }
        void accept(Visitor* visitor) {
            visitor->visit(this);
        }
};

class ArrayConstructorExpr : public ExprNode {
    private:
        list<ExprNode*> exprs;
    public:
        ArrayConstructorExpr(Token tk) : ExprNode(tk) { }
        list<ExprNode*>& getExpressions() {
            return exprs;
        }
        void addExpr(ExprNode* expr) {
            exprs.push_back(expr);
        }
        void accept(Visitor* visit) {
            visit->visit(this);
        }
};

class ConstExpr : public ExprNode {
    public:
        ConstExpr(Token tk) : ExprNode(tk) { }
        ~ConstExpr() { }
        void accept(Visitor* visitor) {
            visitor->visit(this);
        }
};

class SubscriptExpr : public ExprNode {
    private:
        IdExpr* name;
        ExprNode* subscript;
    public:
        SubscriptExpr(Token tk) : ExprNode(tk) { }
        ~SubscriptExpr() {
            delete name;
            delete subscript;
        }
        void setName(IdExpr* expr) {
            name = expr;
        }
        void setSubscript(ExprNode* expr) {
            subscript = expr;
        }
        IdExpr* getName() {
            return name;
        }
        ExprNode* getSubscript() {
            return subscript;
        }
        void accept(Visitor* visitor) {
            visitor->visit(this);
        }
};

class BinaryOpExpr : public ExprNode {
    private:
        ExprNode* leftChild;
        ExprNode* rightChild;
    public:
        BinaryOpExpr(Token tk) : ExprNode(tk) { }
        ~BinaryOpExpr() {
            delete leftChild;
            delete rightChild;
        }
        ExprNode* getLeft() {
            return leftChild;
        }
        ExprNode* getRight() {
            return rightChild;
        }
        void setLeft(ExprNode* lhs) {
            leftChild = lhs;
        }
        void setRight(ExprNode* rhs) {
            rightChild = rhs;
        }
        void accept(Visitor* visitor) {
            visitor->visit(this);
        }
};

class UnaryOpExpr : public ExprNode {
    private:
        ExprNode* expression;
    public:
        UnaryOpExpr(Token tk) : ExprNode(tk) { }
        ~UnaryOpExpr() {
            delete expression;
        }
        void setExpr(ExprNode* expr) {
            expression = expr;
        }
        ExprNode* getExpr() {
            return expression;
        }
        void accept(Visitor* visitor) {
            visitor->visit(this);
        }
};

class FunctionCallExpr : public ExprNode {
    private:
        IdExpr* name;
        ExpressionList* arguments;
    public:
        FunctionCallExpr(Token tk) : ExprNode(tk) { }
        ~FunctionCallExpr() {
            delete name;
            delete arguments;
        }
        void accept(Visitor* visitor) {
            visitor->visit(this);
        }
        void setName(IdExpr* expr) {
            name = expr;
        }
        IdExpr* getName() {
            return name;
        }
        ExpressionList* getArguments() {
            return arguments;
        }
        void setArguments(ExpressionList* exprs) {
            arguments = exprs;
        }
};

class LambdaExpr : public ExprNode {
    private:
        StatementList* params;
        StatementList* body;
    public:
        LambdaExpr(Token tk) : ExprNode(tk) { }
        void accept(Visitor* visitor) {
            visitor->visit(this);
        }
        void setParams(StatementList* exprs) {
            params = exprs;
        }
        void setBody(StatementList* stmts) {
            body = stmts;
        }
        StatementList* getParams() {
            return params;
        }
        StatementList* getBody() {
            return body;
        }
};


>>>>>>> 86f9ac45c8927245ccea6bc7db5f04f076f4188b:src/ast.hpp
#endif