#ifndef context_hpp
#define context_hpp
#include <list>
#include <iostream>
#include <unordered_map>
#include <stack>
#include <vector>
#include "ast.hpp"
#include "token.hpp"
#include "object.hpp"
using namespace std;




struct Scope {
    unordered_map<string, Object> bindings;
    Scope* enclosing;
    Scope* control;
    Scope(Scope* enc = nullptr, Scope* dyn = nullptr) : enclosing(enc), control(dyn) { }
};

struct Function {
    Scope* closure;
    IdExpr* name;
    StatementList* params;
    StatementList* body;
    StatementList* getParams() {
        return params;
    }
    StatementList* getBody() {
        return body;
    }
    IdExpr* getName() {
        return name;
    }
};

//{ let a := "hi"; def showA() { println a; }; { showA(); let a := "verdun"; showA(); println a; } showA(); }

class Context {
    private:
        Scope* scopes;
        Scope* global;
        Object nilInfo;
        Scope* at(int depth) {
            if (depth == -1)
                return global;
            if (depth == 0)
                return scopes;
            Scope* x = scopes;
            while (x != nullptr && depth > 0) {
                x = x->enclosing;
                depth--;
                cout<<".";
            }
            return x;
        }
    public:
        Context() {
            global = new Scope();
            global->enclosing = global;
            global->control = global;
            scopes = global;
        }
        void putAt(string name, Object obj, int depth) {
            cout<<"Put "<<name<<":"<<obj.toString()<<" at depth "<<depth<<endl;
            at(depth)->bindings[name] = obj;
        }
        Object& getAt(string name, int depth) {
            cout<<"Get "<<name<<" at depth "<<depth<<endl;
            Scope* x = at(depth);
            if (x->bindings.find(name) != x->bindings.end())
                return x->bindings[name];
            cout<<"\nHrm.."<<endl;
            return nilInfo;
        }
        Scope* getStack() {
            return scopes;
        }
        void openScope(Scope* s) {
            scopes = s;
        }
        void closeScope() {
            scopes = scopes->control;
        }
};

#endif