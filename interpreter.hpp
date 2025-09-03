#ifndef visitors_hpp
#define visitors_hpp
#include <list>
#include <iostream>
#include <unordered_map>
#include <stack>
#include <vector>
#include "ast.hpp"
#include "token.hpp"
using namespace std;

#define STRING 1
#define NUMBER 2
#define BOOL   3
#define FUNC   4
#define ARRAY  5

struct Object {
    int type;
    union {
        string* strval;
        double numval;
        bool boolval;
        FuncDefStmt* func;
        vector<Object>* arr;
    };
    Object(string s) : type(STRING), strval(new string(s)) { }
    Object(double d = 0) : type(NUMBER), numval(d) { }
    Object(bool b) : type(BOOL), boolval(b) { }
    Object(FuncDefStmt* f) : type(FUNC), func(f) { }
    Object(vector<Object>* a) : type(ARRAY), arr(a) { }
    Object(const Object& o) {
        type = o.type;
        switch (o.type) {
            case 1: strval = o.strval; break;
            case 2: numval = o.numval; break;
            case 3: boolval = o.boolval; break;
            case 4: func = o.func; break;
            case 5: arr = o.arr; break;
        }
    }
    void print() {
        switch (type) {
            case 1: cout<<*strval<<endl; break;
            case 2: cout<<numval<<endl; break;
            case 3: cout<<(boolval ? "true":"false")<<endl; break;
            case 4: cout<<"(func)"<<endl; break;
            case 5: {
                cout<<"["<<endl;
                for (auto m : *arr) {
                     m.print();
                }
                cout<<"]"<<endl;
            } break;
        }
    }
};

struct Scope {
    unordered_map<string, Object> bindings;
    Scope* enclosing;
    Scope(Scope* enc = nullptr) : enclosing(enc) { }
};

class Context {
    private:
        Scope* scopes;
        Scope* global;
        Object nilInfo;
    public:
        Context() {
            global = new Scope(nullptr);
            scopes = global;
        }
        void add(string name, Object obj) {
            scopes->bindings[name] = obj;
        }
        Object& get(string name) {
            Scope* x = scopes;
            while (x != nullptr) {
                if (x->bindings.find(name) != x->bindings.end())
                    return x->bindings[name];
                x = x->enclosing;
            }
            return nilInfo;
        }
        void openScope(Scope* s) {
            s->enclosing = scopes;
            scopes = s;
        }
        void closeScope() {
            scopes = scopes->enclosing;
        }
};

template <class T>
class Stack : public stack<T> {
    public:
        Stack() : stack<T>() {

        }
        T pop() {
            T ret = stack<T>::top();
            stack<T>::pop();
            return ret;
        }
};

class ScopeResolver : public Visitor {
    private:
        unordered_map<string, bool> definitions[255];
        int sp;
        void openScope() {
            sp++;
        }
        void closeScope() {
            --sp;
        }
        bool empty() {
            return sp == 0;
        }
        void declareVarName(string name) {
            if (empty()) {
                return;
            }
            if (definitions[sp-1].find(name) != definitions[sp-1].end()) {
                cout<<name<<" already defined"<<endl;
                return;
            }
            definitions[sp-1][name] = false;
        }
        void defineVarName(string name) {
            if (empty())
                return;
            definitions[sp-1][name] = true;
        }
        void resolveVariableDepth(IdExpr* node, string name) {
            for (int i = sp-1; i > 0; i--) {
                if (definitions[i].find(name) != definitions[i].end()) {
                    
                }
            }
        }
    public:
        ScopeResolver() {
            sp = 0;
        }
};

class Interpreter : public Visitor {
    private:
        Context cxt;
        Stack<Object> sf;
    public:
        Interpreter() {

        }
        void visit(LetStmt* stmt) {
            if (stmt->getExpression()->getToken().getSymbol() == TK_ID) {
                string name = stmt->getExpression()->getToken().getString();
                cxt.add(name, Object());
                cout<<"Added to symbol table: "<<name<<endl;
            }
            stmt->getExpression()->accept(this);
        }
        void visit(IfStmt* stmt) {
            stmt->getPredicate()->accept(this);
            if (sf.pop().boolval) {
                stmt->getTruePath()->accept(this);
            } else {
                if (stmt->getFalsePath()) {
                    stmt->getFalsePath()->accept(this);
                }
            }
        }
        void visit(WhileStmt* stmt) {
            stmt->getPredicate()->accept(this);
            while (sf.pop().boolval) {
                stmt->getBody()->accept(this);
                stmt->getPredicate()->accept(this);
            }
        }
        void visit(FuncDefStmt* stmt) {
            string name = stmt->getName()->getToken().getString();
            cxt.add(name, Object(stmt));
        }
        void visit(ReturnStmt* stmt) {
            stmt->getExpression()->accept(this);
        }
        void visit(StatementList* stmt) {
            for (auto stmt : stmt->getList()) {
                stmt->accept(this);
            }
        }
        void visit(PrintStmt* stmt) {
            stmt->getExpr()->accept(this);
            sf.pop().print();
        }
        void visit(ExprStmt* stmt) {
            stmt->getExpression()->accept(this);
        }
        void visit(ConstExpr* expr) {
            switch (expr->getToken().getSymbol()) {
                case TK_NUMBER: 
                    sf.push(Object(stod(expr->getToken().getString())));
                    break;
                case TK_STRING:
                    sf.push(Object(expr->getToken().getString()));
                    break;
                default:
                    break;
            }
        }
        void visit(IdExpr* expr) {
            sf.push(cxt.get(expr->getToken().getString()));
        }
        void visit(ArrayConstructorExpr* expr) {
            vector<Object>* arr = new vector<Object>();
            for (auto t : expr->getExpressions()) {
                t->accept(this);
                arr->push_back(sf.pop());
            }
            sf.push(Object(arr));
        }
        void visit(SubscriptExpr* expr) {
            expr->getName()->accept(this);
            Object arr = sf.pop();
            expr->getSubsript()->accept(this);
            Object idx = sf.pop();
            sf.push(arr.arr->at(idx.numval));
        }
        void visit(BinaryOpExpr* expr) {
            expr->getLeft()->accept(this);
            Object lhs = sf.pop();
            expr->getRight()->accept(this);
            Object rhs = sf.pop();
            switch (expr->getToken().getSymbol()) {
                case TK_ASSIGN: {
                    string name = expr->getLeft()->getToken().getString();
                    cout<<"Assigning: ";
                    rhs.print();
                    cout<<" to "<<name<<endl;
                    cxt.add(name, rhs);
                } break;
                case TK_LT: {
                    sf.push(Object(lhs.numval < rhs.numval)); break;
                } break;
                case TK_EQ:  {
                    sf.push(Object(lhs.numval == rhs.numval)); break;
                } break;
                case TK_ADD: sf.push(Object(lhs.numval+rhs.numval)); break;
                case TK_SUB: sf.push(Object(lhs.numval-rhs.numval)); break;
                case TK_MUL: sf.push(Object(lhs.numval*rhs.numval)); break;
                case TK_DIV: sf.push(Object(lhs.numval/rhs.numval)); break;
            }
        }
        void visit(UnaryOpExpr* expr) {
            expr->getExpr()->accept(this);
            Object v = sf.pop();
            v.numval = -v.numval;
            sf.push(v);
        }
        void visit(FunctionCallExpr* expr) {
            expr->getName()->accept(this);
            Scope* scope = new Scope();
            auto func = sf.pop();
            if (func.type != FUNC) {
                cout<<"Error not a function"<<endl;
                return;
            }
            auto params = func.func->getParams()->getList();
            auto args = expr->getArguments()->getExpressions();
            auto param = params.begin();
            auto arg = args.begin();
            while (param != params.end() && arg != args.end()) {
                (*arg)->accept(this);
                Object val = sf.pop();
                string name = ((IdExpr*)((LetStmt*)(*param))->getExpression())->getToken().getString();
                scope->bindings[name] = val;
                //cout<<"Bound "<<name<<" to value"<<endl;
                param++;
                arg++;
            }
            if (param != params.end() || arg != args.end()) {
                cout<<"Error: function call has mismatched arguments."<<endl;
            }
            cxt.openScope(scope);
            func.func->getBody()->accept(this);
            cxt.closeScope();
        }
        void visit(ExpressionList* exprs) {
            for (auto e : exprs->getExpressions()) {
                e->accept(this);
            }
        }
};



#endif