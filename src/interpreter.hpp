#ifndef visitors_hpp
#define visitors_hpp
#include "context.hpp"
#include "resolvescope.hpp"
#include <cmath>
#include "re/re_compiler.hpp"
#include "re/subset_match.hpp"

class ReturnStmtException : public std::exception {
    virtual const char* what() const throw() {
        return "return stmt";
    }
};


//def cd(let k) { if (k < 10) { println k; k := k + 1; cd(k); } else { println "dine"; } }; cd(5);

class Interpreter : public Visitor {
    private:
        Context cxt;
        InsepctableStack<Object> sf;
        Object add(Object lhs, Object rhs) {
            if (lhs.type == NUMBER && rhs.type == NUMBER)
                return Object(lhs.numval + rhs.numval);
            return Object(string(lhs.toString() + rhs.toString()));
        }
        Object sub(Object lhs, Object rhs) {
            if (lhs.type != NUMBER)
                return lhs;
            if (rhs.type != NUMBER)
                return rhs;
            return Object(lhs.numval - rhs.numval);
        }
        void handleSubscriptAssignment(BinaryOpExpr* expr, Object rhs) {
            auto x = dynamic_cast<SubscriptExpr*>(expr->getLeft());
            x->getName()->accept(this);
            Object m = sf.pop();
            x->getSubsript()->accept(this);
            int pos = sf.pop().numval;
            switch (expr->getToken().getSymbol()) {
                case TK_ASSIGN: m.arr->at(pos) = rhs; break;
                case TK_ASSIGN_SUM: m.arr->at(pos) = add(m.arr->at(pos), rhs); break;
                case TK_ASSIGN_DIFF: m.arr->at(pos) = sub(m.arr->at(pos), rhs); break;
            }
        }
        void handleAssignment(BinaryOpExpr* expr, Object rhs) {
            string name = expr->getLeft()->getToken().getString();
            if (SubscriptExpr* ss = dynamic_cast<SubscriptExpr*>(expr->getLeft())) {
                handleSubscriptAssignment(expr, rhs);
            } else {
                //cout<<"Assigning: "<<rhs.toString()<<" to "<<name<<" at depth: "<<expr->getLeft()->getToken().scopeLevel()<<endl;
                Object lhs = cxt.getAt(name, expr->getLeft()->getToken().scopeLevel());
                switch (expr->getToken().getSymbol()) {
                    case TK_ASSIGN: break;
                    case TK_ASSIGN_SUM: rhs = add(lhs, rhs); break;
                    case TK_ASSIGN_DIFF: rhs = sub(lhs, rhs); break;
                }
                cxt.putAt(name, rhs, expr->getLeft()->getToken().scopeLevel());
                
            }
        }
        void handleRegExMatch(Object txt, Object pat) {
            RECompiler cmp;
            REParser prs;
            string text = txt.toString();
            text = text.substr(1, text.length()-2);
            string pattern = pat.toString();
            pattern = pattern.substr(1, pattern.length()-2);
            cout<<"Looking for "<<pattern<<" in "<<text<<endl;
            NFA nfa = cmp.compile(prs.parse(pattern));
            sf.push(Object(match(nfa, text)));
        }
        Scope* evaluateArguments(Function* func, list<ExprNode*> args) {
            Scope* scope = new Scope(func->closure, cxt.getStack());
            list<StmtNode*> params = func->getParams()->getList();
            auto param = params.begin();
            auto arg = args.begin();
            while (param != params.end() && arg != args.end()) {
                (*arg)->accept(this);
                Object val = sf.pop();
                string name = ((IdExpr*)((LetStmt*)(*param))->getExpression())->getToken().getString();
                //cout<<"Binding "<<val.toString()<<" to "<<name<<endl;
                scope->bindings[name] = val;
                param++; arg++;
            }
            if (param != params.end() || arg != args.end()) {
                cout<<"Error: function call has mismatched arguments."<<endl;
            }
            return scope;
        }
        void applyFunction(Function* func, Scope* env) {
            cxt.openScope(env);
            try {
                func->getBody()->accept(this);
            } catch (ReturnStmtException rse) {

            }
            cxt.closeScope();
        }
        void applyBinaryOperator(BinaryOpExpr* expr, Object& lhs, Object& rhs) {
            //cout<<lhs.toString()<<" "<<expr->getToken().getString()<<" "<<rhs.toString()<<endl;
            switch (expr->getToken().getSymbol()) {
                case TK_ASSIGN_SUM:
                case TK_ASSIGN_DIFF:
                case TK_ASSIGN: handleAssignment(expr, rhs); break;
                case TK_MATCHRE: handleRegExMatch(lhs, rhs);
                case TK_LT: sf.push(Object(lhs.numval < rhs.numval)); break;
                case TK_GT: sf.push(Object(lhs.numval > rhs.numval)); break;
                case TK_EQ: sf.push(Object(lhs.numval == rhs.numval)); break;
                case TK_LTE: sf.push(Object(lhs.numval <= rhs.numval)); break;
                case TK_GTE: sf.push(Object(lhs.numval >= rhs.numval)); break;
                case TK_NEQ: sf.push(Object(lhs.numval != rhs.numval)); break;
                case TK_ADD: sf.push(Object(lhs.numval + rhs.numval));break;
                case TK_SUB: sf.push(Object(lhs.numval - rhs.numval)); break;
                case TK_MUL: sf.push(Object(lhs.numval * rhs.numval));break;
                case TK_DIV: sf.push(Object(lhs.numval / rhs.numval)); break;
                case TK_MOD: sf.push(Object(std::fmod(lhs.numval, rhs.numval))); break;
                case TK_AND: sf.push(Object(lhs.boolval && rhs.boolval)); break;
                case TK_OR: sf.push(Object(lhs.boolval ||  rhs.boolval)); break;
                default:
                    break;
            }
        }
        void doPush(ListOpExpr* expr, Object& m) {
            expr->getExpr()->accept(this);
            m.arr->insert(m.arr->begin(), sf.pop());
            sf.push(m);
        }
        void doPop(Object& m) {
            Object t = m.arr->at(0);
            m.arr->erase(m.arr->begin());
            sf.push(t);
        }
        void doAppend(ListOpExpr* expr, Object& m) {
            expr->getExpr()->accept(this);
            m.arr->push_back(sf.pop());
            sf.push(m);
        }
        void doGet(ListOpExpr* expr, Object& m) {
            expr->getExpr()->accept(this);
            sf.push(m.arr->at(sf.pop().numval));
        }
        void doCdr(Object& m) {
            vector<Object>* obj = new vector<Object>();
            for (int i = 1; i < m.arr->size(); i++) {
                obj->push_back(m.arr->at(i));
            }
            sf.push(Object(obj));
        }
        void doMap(ListOpExpr* expr, Object& m) {
            expr->getExpr()->accept(this);
            Object lmb = sf.pop();
            if (lmb.type == FUNC) {
                vector<Object>* res = new vector<Object>();
                for (Object t : *(m.arr)) {
                    Scope* scope = new Scope(lmb.func->closure, cxt.getStack());
                    string name = lmb.func->getParams()->getList().front()->getToken().getString();
                    //cout<<"Binding "<<t.toString()<<" to "<<name<<endl;
                    scope->bindings[name] = t;
                    applyFunction(lmb.func,scope);
                    res->push_back(sf.pop());
                } 
                sf.push(Object(res));
            } else {
                cout<<"Error: map expects a function argument"<<endl;
            }
        }
        void doFilter(ListOpExpr* expr, Object& m) {
            expr->getExpr()->accept(this);
            Object lmb = sf.pop();
            if (lmb.type == FUNC) {
                vector<Object>* res = new vector<Object>();
                for (Object t : *(m.arr)) {
                    Scope* scope = new Scope(lmb.func->closure, cxt.getStack());
                    string name = lmb.func->getParams()->getList().front()->getToken().getString();
                    //cout<<"Binding "<<t.toString()<<" to "<<name<<endl;
                    scope->bindings[name] = t;
                    applyFunction(lmb.func, scope);
                    if (sf.pop().boolval)
                        res->push_back(t);
                } 
                sf.push(Object(res));
            } else {
                cout<<"Error: map expects a function argument"<<endl;
            }
        }
    public:
        Interpreter() {

        }
        void visit(LetStmt* stmt) {
            if (stmt->getExpression()->getToken().getSymbol() == TK_ID) {
                string name = stmt->getExpression()->getToken().getString();
                int depth = stmt->getExpression()->getToken().scopeLevel();
                cxt.putAt(name, Object(), depth);
                //cout<<"Added to symbol table: "<<name<<endl;
            }
            stmt->getExpression()->accept(this);
        }
        void visit(IfStmt* stmt) {
            //cout<<"If Stmt"<<endl;
            stmt->getPredicate()->accept(this);
            if (!sf.empty() && sf.pop().boolval) {
               // cout<<"took True path"<<endl;
                stmt->getTruePath()->accept(this);
            } else {
               // cout<<"took false path"<<endl;
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
        void visit(ReturnStmt* stmt) {
            stmt->getExpression()->accept(this);
            throw ReturnStmtException();
        }
        void visit(FuncDefStmt* stmt) {
            string name = stmt->getName()->getToken().getString();
            int depth = stmt->getName()->getToken().scopeLevel();
            Function* func = new Function();
            func->body = stmt->getBody();
            func->params = stmt->getParams();
            func->name = stmt->getName();
            func->closure = cxt.getStack();
            cxt.putAt(name, Object(func), depth);
        }
        void visit(LambdaExpr* expr) {
            Function* func = new Function();
            func->body = expr->getBody();
            func->params = expr->getParams();
            func->name = new IdExpr(Token(TK_DEF, "Lambda"));
            func->closure = cxt.getStack();
            sf.push(Object(func));
        }
        void visit(FunctionCallExpr* expr) {
            expr->getName()->accept(this);
            auto func = sf.pop();
            if (func.type != FUNC) {
                cout<<"Error not a function"<<endl;
                return;
            }
            auto args = expr->getArguments()->getExpressions();
            // eval <-> apply
            Scope* env = evaluateArguments(func.func, args);
            applyFunction(func.func, env);
        }
        void visit(IdExpr* expr) {
            sf.push(cxt.getAt(expr->getToken().getString(), expr->getToken().scopeLevel()));
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
            applyBinaryOperator(expr, lhs, rhs);
        }
        void visit(UnaryOpExpr* expr) {
            expr->getExpr()->accept(this);
            Object v = sf.pop();
            switch (expr->getToken().getSymbol()) {
                case TK_SUB: v.numval = -v.numval; break;
                case TK_INCREMENT: v.numval += 1; break;
                case TK_DECREMENT: v.numval -= 1; break;
            }
            sf.push(v);
            string name = expr->getExpr()->getToken().getString();
            int depth = expr->getExpr()->getToken().scopeLevel();
            cxt.putAt(name, v, depth);
        }
        void visit(ListOpExpr* expr) {
            expr->getList()->accept(this);
            Object m = sf.pop();
            if (m.type != ARRAY) {
                cout<<"Error: "<<expr->getToken().getString()<<" expects a list."<<endl;
                sf.push(Object());
            }
            switch (expr->getToken().getSymbol()) {
                case TK_EMPTY: {  sf.push(Object(m.arr->empty())); } break;
                case TK_SIZE:  {  sf.push(Object((double)m.arr->size())); } break;
                case TK_FIRST: {  sf.push(m.arr->at(0)); } break;
                case TK_MAP:   {  doMap(expr, m);    } break;
                case TK_FILTER: { doFilter(expr, m); } break;
                case TK_POP:    { doPop(m);        } break;
                case TK_REST:   { doCdr(m);          } break;
                case TK_APPEND: { doAppend(expr, m); } break;
                case TK_GET:    { doGet(expr, m);    } break;
                case TK_PUSH:   { doPush(expr, m);   } break;
                case TK_REDUCE: {  } break;
                default:
                    break;
            }
        }
        void visit(ConstExpr* expr) {
            switch (expr->getToken().getSymbol()) {
                case TK_NUMBER: sf.push(Object(stod(expr->getToken().getString()))); break;
                case TK_STRING: sf.push(Object(expr->getToken().getString())); break;
                case TK_TRUE:   sf.push(Object(true)); break;
                case TK_FALSE:  sf.push(Object(false)); break;
                default:
                    break;
            }
        }
        void visit(BlockStmt* stmt) {
            Scope* ar = new Scope(cxt.getStack(), cxt.getStack());
            cxt.openScope(ar);
            stmt->getStatements()->accept(this);
            cxt.closeScope();
        }
        void visit(ExpressionList* exprs) {
            for (auto e : exprs->getExpressions()) {
                e->accept(this);
            }
        }
        void visit(ObjectConstructorExpr* expr) {
            expr->getName()->accept(this);
            for (auto t : expr->getExpressions()) {
                t->accept(this);
            }
        }
        void visit(ObjectDefStmt* stmt) {
            ClassObject* t = new ClassObject();
            t->setName(stmt->getName());
            t->setBody(stmt->getBody());
            string name = stmt->getName()->getToken().getString();
            cxt.addClassDef(name, t);
            cout<<name<<" defined."<<endl;
        }
};

#endif