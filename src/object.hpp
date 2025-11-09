#ifndef object_hpp
#define object_hpp
#include <iostream>
#include "ast.hpp"
using namespace std;

 enum ObjectType {
    STRING = 1,
    NUMBER = 2,
    BOOL   = 3,
    FUNC   = 4,
    ARRAY  = 5,
    OBJECT = 6,
    POINTER  = 7,
    NIL    = 8
};

struct Scope;

class Function {
    private:
        friend class Interpreter;
        Scope* closure;
        IdExpr* name;
        StatementList* params;
        StatementList* body;
    public:
        Function() {

        }
        StatementList* getParams() {
            return params;
        }
        StatementList* getBody() {
            return body;
        }
        IdExpr* getName() {
            return name;
        }
        Scope* getClosure() {
            return closure;
        }
};

class ClassObject {
    private:
        friend class Interpreter;
        Scope* scope;
        Scope* members;
        IdExpr* name;
        StatementList* body;
    public:
        ClassObject() {

        }
        void setBody(StatementList* s) {
            body = s;
        }
        void setName(IdExpr* expr) {
            name = expr;
        }
        void setParentScope(Scope* s) {
            scope = s;
        }
        StatementList* getBody() {
            return body;
        }
        IdExpr* getName() {
            return name;
        }
        Scope* getEnv() {
            return members;
        }
};

struct Object {
    ObjectType type;
    bool marked;
    union {
        string* strval;
        double numval;
        bool boolval;
        Function* func;
        ClassObject* clazz;
        vector<Object>* arr;
        Object* obj;
    };
    Object(string s) : type(STRING), strval(new string(s)), marked(false) { }
    Object(double d) : type(NUMBER), numval(d), marked(false) { }
    Object(bool b) : type(BOOL), boolval(b), marked(false) { }
    Object(Function* f) : type(FUNC), func(f), marked(false) { }
    Object(vector<Object>* a) : type(ARRAY), arr(a), marked(false) { }
    Object(Object* o) : type(POINTER), obj(o), marked(false) { }
    Object(ClassObject* o) : type(OBJECT), clazz(o), marked(false) { }
    Object() : type(NIL), numval(0), marked(false) { } 
    Object(const Object& o) {
        type = o.type;
        marked = o.marked;
        switch (o.type) {
            case STRING: strval = o.strval; break;
            case NUMBER: numval = o.numval; break;
            case BOOL: boolval = o.boolval; break;
            case FUNC: func = o.func; break;
            case ARRAY: arr = o.arr; break;
        }
    }
    Object& operator=(const Object& o) {
        if (this != &o) {
            type = o.type;
            marked = o.marked;
            switch (o.type) {
                case STRING: strval = o.strval; break;
                case NUMBER: numval = o.numval; break;
                case BOOL: boolval = o.boolval; break;
                case FUNC: func = o.func; break;
                case ARRAY: arr = o.arr; break;
            }
        }
        return *this;
    }
    string toString() {
        switch (type) {
            case STRING: return *strval; 
            case NUMBER: return to_string(numval);
            case BOOL: return (boolval ? "true":"false");
            case FUNC:  return "(func)";
            case OBJECT: return "(object)";
            case ARRAY: {
                string asStr = "[ ";
                for (auto m : *arr) {
                    asStr += m.toString() + " ";
                }
                asStr += "]";
                return asStr;
            } break;
            case POINTER: {
                string asStr = "Pointer to -> " + obj->toString();
                return asStr;
            }
        }
        return "null";
    }
    void print() {
        cout<<this->toString()<<endl;
    }
};

#endif