#ifndef object_hpp
#define object_hpp
#include <iostream>
using namespace std;

 enum ObjectType {
    STRING = 1,
    NUMBER = 2,
    BOOL   = 3,
    FUNC   = 4,
    ARRAY  = 5
};

struct Object;

Object add(Object a, Object b);
Object sub(Object a, Object b);
Object div(Object a, Object b);
Object mul(Object a, Object b);
Object lt(Object a, Object b);
Object lte(Object a, Object b);
Object gt(Object a, Object b) ;
Object equ(Object a, Object b);
Object neq(Object a, Object b);

struct Function;

struct Object {
    ObjectType type;
    union {
        string* strval;
        double numval;
        bool boolval;
        Function* func;
        vector<Object>* arr;
    };
    Object(string s) : type(STRING), strval(new string(s)) { }
    Object(double d = 0) : type(NUMBER), numval(d) { }
    Object(bool b) : type(BOOL), boolval(b) { }
    Object(Function* f) : type(FUNC), func(f) { }
    Object(vector<Object>* a) : type(ARRAY), arr(a) { }
    Object(const Object& o) {
        type = o.type;
        switch (o.type) {
            case STRING: strval = o.strval; break;
            case NUMBER: numval = o.numval; break;
            case BOOL: boolval = o.boolval; break;
            case FUNC: func = o.func; break;
            case ARRAY: arr = o.arr; break;
        }
    }
    string toString() {
        switch (type) {
            case STRING: return *strval; 
            case NUMBER: return to_string(numval);
            case BOOL: return (boolval ? "true":"false");
            case FUNC:  return "(func)";
            case ARRAY: {
                string asStr = "[ ";
                for (auto m : *arr) {
                    asStr += m.toString() + " ";
                }
                asStr += "]";
                return asStr;
            } break;
        }
        return "null";
    }
    void print() {
        cout<<this->toString()<<endl;
    }
    int compareTo(Object b) {
        if (lt(this, b).boolval)
            return -1;
        if (gt(this, b).boolval)
            return 1;
        return neq(this, b).boolval;
    }
};

Object add(Object a, Object b) {
    if (a.type == STRING && b.type == STRING) {
        return Object((*a.strval) + (*b.strval));
    }
    if (a.type == NUMBER && b.type == NUMBER) {
        return Object(a.numval + b.numval);
    }
    return Object();
}

Object sub(Object a, Object b) {
    if (a.type == NUMBER && b.type == NUMBER) {
        return Object(a.numval - b.numval);
    }
    return Object();
}

Object div(Object a, Object b) {
    if (a.type == NUMBER && b.type == NUMBER) {
        return Object(a.numval / b.numval);
    }
    return Object();
}

Object mul(Object a, Object b) {
    if (a.type == NUMBER && b.type == NUMBER) {
        return Object(a.numval * b.numval);
    }
    return Object();
}

Object lt(Object a, Object b) {
    if (a.type == NUMBER && b.type == NUMBER) {
        return Object(a.numval < b.numval);
    }
    return Object(a.toString() < b.toString());
}

Object gt(Object a, Object b) {
    if (a.type == NUMBER && b.type == NUMBER) {
        return Object(a.numval > b.numval);
    }
    return Object(a.toString() > b.toString());
}

Object equ(Object a, Object b) {
    if (a.type == NUMBER && b.type == NUMBER) {
        return Object(a.numval == b.numval);
    }
    return Object(a.toString() == b.toString());
}

Object neq(Object a, Object b) {
    return Object(!equ(a, b).boolval);
}

Object lte(Object a, Object b) {
    if (a.type == NUMBER && b.type == NUMBER) {
        return Object(a.numval <= b.numval);
    }
    return Object(a.toString() <= b.toString());
}

Object gte(Object a, Object b) {
    if (a.type == NUMBER && b.type == NUMBER) {
        return Object(a.numval >= b.numval);
    }
    return Object(a.toString() >= b.toString());
}

#endif