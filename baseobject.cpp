#include <iostream>
#include <vector>
#include "src/ast.hpp"
#include "src/context.hpp"
using namespace std;

class Object {
    public:
        enum class Type {
            STRING,
            NUMBER,
            BOOL,
            FUNC,
            ARRAY,
            OBJECT,
            NIL
        };
        virtual ~Object() = default;
        virtual Type getType() const = 0;
        virtual std::string toString() const = 0;
};

class NumberObject : public Object {
    private:
        double numval;
    public:
        NumberObject(double val) : numval(val) { }
        Type getType() const override { return Type::Number; }
        std::string toString() const { return to_string(numval); }
        double eval() { return numval; }
};

class StringObject : public Object {
    private:
        string value;
    public:
        StringObject(string val) : value(val) { }
        Type getType() const override { return Type::String; }
        std::string toString() const override { return value; }
        string eval() { return value; }
};

class BoolObject : public Object {
    private:
        bool boolval;
    public:
        BoolObject(bool val) : boolval(val) { }
        Type getType() const override { return Type::Bool; }
        std::string toString() { return boolval ? "true":"false"; }
        bool eval() { return boolval; }
};

/*
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
*/

class FuncObject : public Object { 
    private:
        Function* func;
    public:
        FuncObject(Function* fn) : func(fn) { }
        Type getType() const override { return Type::Func; }
        std::string toString() { return "func"; }
        Function* eval() { return func; }
};


class ArrayObject : public Object {
    private:
        vector<Object*> arr;
    public:
        ArrayObject(vector<Object*> ar) : arr(ar) {

        }
        Type getType() const override { return Type::Bool; }
        std::string toString() const { 
            string str = "[";
            for (auto obj : arr) {
                str += obj->toString();
            }
            str += "]";
            return str;
         }
        vector<Object*> eval() { return arr; }
};

/*
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
};
*/
class ClazzObject : public Object {
    private:
        ClassObject* object;
    public:
        ClazzObject(ClassObject* co) : object(co) {

        }
        Type getType() const override { return Type::Object; }
        std::string toString() { return "object"; }
        ClassObject* eval() { return object; }
};

void printObject(Object* bo) {
    cout<<bo->toString()<<endl;
}

int main() {
    NumberObject* t = new NumberObject(15);
    printObject(t);
    StringObject* st = new StringObject("But monkweh!?");
    printObject(st);
    vector<Object*> ov = { t, st };
    ArrayObject* at = new ArrayObject(ov);
    printObject(at); 
}