#ifndef object_hpp
#define object_hpp
#include <iostream>
#include <cmath>
#include "../parse/ast.hpp"
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

class ClassObject;
string objToString(ClassObject* o);
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
    Object(string s) : type(ObjectType::STRING), strval(new string(s)), marked(false) { }
    Object(double d) : type(ObjectType::NUMBER), numval(d), marked(false) { }
    Object(bool b) : type(ObjectType::BOOL), boolval(b), marked(false) { }
    Object(Function* f) : type(ObjectType::FUNC), func(f), marked(false) { }
    Object(vector<Object>* a) : type(ObjectType::ARRAY), arr(a), marked(false) { }
    Object(ClassObject* o) : type(ObjectType::OBJECT), clazz(o), marked(false) { }
    Object() : type(ObjectType::NIL), numval(0), marked(false) { } 
    Object(const Object& o) {
        type = o.type;
        marked = o.marked;
        switch (o.type) {
            case STRING: strval = o.strval; break;
            case NUMBER: numval = o.numval; break;
            case BOOL: boolval = o.boolval; break;
            case FUNC: func = o.func; break;
            case ARRAY: arr = o.arr; break;
            case OBJECT: clazz = o.clazz; break;
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
                case OBJECT: clazz = o.clazz; break;
            }
        }
        return *this;
    }
    string toString() {
        switch (type) {
            case STRING: return *strval; 
            case NUMBER: {
                if (numval == floor(numval)) 
                    return to_string((int)numval);
                return to_string(numval);
            } break;
            case BOOL: return (boolval ? "true":"false");
            case FUNC:  return "(func)";
            case OBJECT: {
                return objToString(clazz);
            } break;
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

class ClassObject {
    private:
        friend class Interpreter;
        string typeName;
        bool instantiated;
        unordered_map<string, Object> fields;
    public:
        ClassObject() {

        }
        void setName(string n) {
            typeName = n;
        }
        string getTypeName() {
            return typeName;
        }
        bool isAlive() {
            return instantiated;
        }
        Object getMember(string name) {
            return fields[name];
        }
        void setMember(string name, Object m) {
            fields[name] = m;
        }
        unordered_map<string, Object>& getFields() {
            return fields;
        }
        string toString() {
            string str = "{";
            for (auto m : fields) {
                str += "[ " + m.first +": " + m.second.toString()+" ] ";
            }
            str += "}";
            return str;
        }
};

string objToString(ClassObject* o) {
    return o == nullptr ? "nil":o->toString();
}

Object add(Object lhs, Object rhs) {
    if (lhs.type == NUMBER && rhs.type == NUMBER)
        return Object(lhs.numval + rhs.numval);
    cout<<"Adding string wise."<<endl;
    return Object(string(lhs.toString() + rhs.toString()));
}
Object sub(Object lhs, Object rhs) {
    if (lhs.type != NUMBER)
        return lhs;
    if (rhs.type != NUMBER)
        return rhs;
    return Object(lhs.numval - rhs.numval);
}
Object gt(Object lhs, Object rhs) {
    if (lhs.type != rhs.type)
        return Object(false);
    switch (lhs.type) {
        case NUMBER: return Object(lhs.numval > rhs.numval);
        case BOOL: return Object(lhs.boolval > rhs.boolval);
        case STRING: return Object(lhs.toString() > rhs.toString());
    }
    return Object(false);
}
Object equ(Object lhs, Object rhs) {
    if (lhs.type != rhs.type)
        return Object(false);
    switch (lhs.type) {
        case NUMBER: return Object(lhs.numval == rhs.numval);
        case BOOL: return Object(lhs.boolval == rhs.boolval);
        case STRING: return Object(lhs.toString() == rhs.toString());
        case ARRAY: return Object(lhs.toString() == rhs.toString());
        case OBJECT: {
            if (lhs.clazz->getTypeName() != rhs.clazz->getTypeName()) {
                cout<<"Not even the same type!"<<endl;
                return Object(false);
            }
            for (auto m : lhs.clazz->getFields()) {
                Object q =  rhs.clazz->getMember(m.first);
                if (equ(m.second, q).boolval == false) {
                    cout<<"Failed on "<<m.first<<endl;
                    return Object(false);
                }
            }
            cout<<"They match!"<<endl;
            return Object(true);
        } break;
        case NIL: return Object(true);
    }
    return Object(false);
}
Object lt(Object lhs, Object rhs) {
    return gt(rhs, lhs);
}
Object lte(Object lhs, Object rhs) {
    return lt(lhs,rhs).boolval || equ(lhs, rhs).boolval;
}
Object gte(Object lhs, Object rhs) {
    return gt(lhs, rhs).boolval || equ(lhs, rhs).boolval;
}
Object neq(Object lhs, Object rhs) {
    return equ(lhs, rhs).boolval == false;
}

#endif