class BaseObject {
    public:
        enum class Type {
            String,
            Number,
            Bool,
            Func,
            Array,
            Null,
        };
        virtual ~BaseObject() = default;
        virtual Type getType() const = 0;
        virtual std::string toString() const = 0;
};

class NumberObject : public BaseObject {
    private:
        double numval;
    public:
        NumberObject(double val) : numval(val) { }
        Type getType() const override { return Type::Number; }
        std::string toString() { return to_string(numval); }
        double eval() { return numval; }
};

class StringObject : public BaseObject {
    private:
        string value;
    public:
        StringObject(string val) : value(val) { }
        Type getType() const override { return Type::String; }
        std::string toString() const override { return value; }
        string eval() { return value; }
};

class BoolObject : public BaseObject {
    private:
        bool boolval;
    public:
        BoolObject(bool val) : boolval(val) { }
        Type getType() const override { return Type::Bool; }
        std::string toString() { return boolval ? "true":"false"; }
        bool eval() { return boolval; }
};

class FuncObject : public BaseObject { 
    private:
        FuncDefStmt* func;
    public:
        FuncObject(FuncDefStmt* fn) : func(fn) { }
        Type getType() const override { return Type::Func; }
        std::string toString() { return "func" }
        FuncDefStmt* eval() { return func; }
};

class ArraryObject : public BaseObject {
    private:
        vector<BaseObject*> arr;
    public:
        ArrayObject(vector<BaseObject*> ar) : arr(ar) {

        }
        Type getType() const override { return Type::Bool; }
        std::string toString() { 
            string str = "[";
            for (auto obj : arr) {
                str += obj->toString();
            }
            str += "]";
            return str;
         }
        vector<BaseObject*> eval() { return arr; }
};