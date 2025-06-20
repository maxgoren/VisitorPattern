#ifndef visitors_hpp
#define visitors_hpp
#include <vector>
#include <list>
#include "token.hpp"
#include "syntaxtree.hpp"
using namespace std;


class PrintVisitor : public Visitor {
    private:
        int d;
        void enter(string s = "") {
            ++d;
            say(s);
        }
        void leave(string s = "") {
            --d;
        }
        void say(string s) {
            for (int i = 0; i < d; i++) {
                cout<<"  ";
            }
            cout<<s<<endl;
        }
    public:
        void visit(StatementList* sl) override {
            for (auto m : sl->getStatements()) {
                m->accept(this);
            }
        }
        void visit(ProgramStatement* ps) override {
            enter("Program");
            ps->getStatement()->accept(this);
            leave();
        }
        void visit(PrintStatement* ps) override {
            enter("Print statement");
            ps->getExpression()->accept(this);
            leave();
        }
        void visit(WhileStatement* ws) override {
            enter("While statement");
            ws->getTestExpr()->accept(this);
            ws->getLoopBody()->accept(this);
            leave();
        }
        void visit(IfStatement* is) override {
            enter("if statement");
            is->getTest()->accept(this);
            is->getPassCase()->accept(this);
            if (is->getFailCase() != nullptr) {
                is->getFailCase()->accept(this);
            }
            leave();
        }
        void visit(ExprStatement* es) override {
            enter("Expr Statement");
            if (es->getExpression() != nullptr)
                es->getExpression()->accept(this);
            leave();
        }
        void visit(IdExpression* idexpr) override {
            enter("Id Expression");
            say(idexpr->getToken().lexeme);
            leave();
        }
        void visit(LiteralExpression* lit) override {
            enter("Literal Expression");
            say(lit->getToken().lexeme);
            leave();
        }
        void visit(AssignExpression* assign) override {
            enter("Assignment Expression");
            assign->getLeft()->accept(this);
            assign->getRight()->accept(this);
            leave();
        }
        void visit(BinaryExpression* bin) override {
            enter("Binary Expression");
            say(bin->getToken().lexeme);
            bin->getLeft()->accept(this);
            bin->getRight()->accept(this);
            leave();
        }
        void visit(UnaryExpression* unary) override {
            enter("unary expr");
            say(unary->getToken().lexeme);
            unary->getLeft()->accept(this);
            leave();
        }
         void visit(RelOpExpression* rel) override {
            enter("Relop Expression");
            say(rel->getToken().lexeme);
            rel->getLeft()->accept(this);
            rel->getRight()->accept(this);
            leave();
        }
        void visit(FuncDefStatement* ds) override {
            enter("Function Definition");
            say(ds->getName());
            ds->getParams()->accept(this);
            ds->getBody()->accept(this);
            leave();
        }
        void visit(ParameterList* pl) override {
            enter("Parameter List");
            for (auto m : pl->getParams()) {
                m->accept(this);
            }
            leave();
        }
        void visit(FunctionCall* fc) override {
            enter("Function call");
            fc->getName()->accept(this);
            for (auto it : fc->getArgs()) {
                it->accept(this);
            }
            leave();
        }
        void visit(ReturnStatement* rs) override {
            enter("return statement");
            rs->getRetVal()->accept(this);
            leave();
        }
        void visit(VarDefStatement* vd) override {
            enter("Variable Definition");
            say(vd->getName());
            if (vd->getExpr() != nullptr) {
                vd->getExpr()->accept(this);
            }
            leave();
        }
        void visit(ListExpression* le) override {
            enter("list expression");
            for (auto m : le->getExprsList()) {
                if (m != nullptr)
                    m->accept(this);
            }
            leave();
        }
        void visit(SubscriptExpression* se) override {
            enter("subscript expression");
            se->getName()->accept(this);
            se->getPosition()->accept(this);
            leave();
        }
}; 

class Function {
    private:
        string name;
        ParameterList* params;
        StatementList* body;
    public:
        Function(string n, ParameterList* p, StatementList* b) : name(n), params(p), body(b) { }
        string getName() { return name; }
        ParameterList* paramList() { return params; }
        StatementList* getBody() { return body; }
};

class InterpreterVisitor : public Visitor {
    private:
        bool bailout;
        vector<Environment> envs;
        Environment env;
        Object nilObject;
        Object operands[31337];
        int n = 0;
        void push(Object e) {
            operands[n++] = e;
        }
        Object pop() {
            if (n > 0)
                return operands[--n];
            cout<<"Stack underflow."<<endl;
            return nilObject;
        }
        Object peek(int k) {
            return operands[(n-1)-k];
        }
        /* This is quite possibly *the least* efficient way of doing this, 
           it is quite literally for demonstration purposes only. */
        void openScope() {
            envs.push_back(env);
            env = Environment();
        }
        void closeScope() {
            if (!envs.empty()) {
                env = envs.back();
                envs.pop_back();
            }
        }
    public:
        void visit(StatementList* sl) override {
            for (auto m : sl->getStatements()) {
                m->accept(this);
                if (bailout) {
                    break;
                }
            }
        }
        void visit(ProgramStatement* ps) override {
            ps->getStatement()->accept(this);
        }
        void visit(PrintStatement* ps) override {
            ps->getExpression()->accept(this);
            cout<<pop()<<endl;
        }
        void visit(WhileStatement* ws) override {
            ExpressionNode* testExpr = ws->getTestExpr();
            StatementList* stmt = ws->getLoopBody();
            for (;;) {
                testExpr->accept(this);
                if (pop().boolval) {
                    stmt->accept(this);
                } else break;
            }
        }
        void visit(IfStatement* is) override {
            is->getTest()->accept(this);
            if (pop().boolval) {
                is->getPassCase()->accept(this);
            } else {
                if (is->getFailCase() != nullptr)
                    is->getFailCase()->accept(this);
            }
        }
        void visit(VarDefStatement* vd) override {
            env[vd->getName()] = nilObject;
            if (vd->getExpr() != nullptr) {
                vd->getExpr()->accept(this);
            }
        }
        void visit(ExprStatement* es) override {
            if (es->getExpression() != nullptr)
                es->getExpression()->accept(this);
        }
        void visit(UnaryExpression* unary) override {
            unary->getLeft()->accept(this);
            Object t = pop();
            t.numval = -t.numval;
            push(t);
        }
        void visit(IdExpression* idexpr) override {
            string id = idexpr->getId();
            if (env.find(id) == env.end()) {
                for (int i = envs.size()-1; i >= 0; i--) {
                    if (envs[i].find(id) != envs[i].end()) {
                        push(envs[i].at(id));
                        return;
                    }
                }
                cout<<"Hmmm, couldnt find: "<<id<<endl;
            } else {
                push(env[idexpr->getId()]);
            }
        }
        void visit(LiteralExpression* lit) override {
            push(Object(lit->eval(env)));
        }
        void visit(AssignExpression* assign) override {
            string id = assign->getLeft()->getToken().lexeme;
            if (id == "[") {
                auto x = dynamic_cast<SubscriptExpression*>(assign->getLeft());
                while (x->getName()->getId() == "[") x = dynamic_cast<SubscriptExpression*>(x->getName());
                x->getName()->accept(this);
                Object m = pop();
                x->getPosition()->accept(this);
                int pos = pop().numval;
                assign->getRight()->accept(this);
                Object ans = pop();
                m.vec->at(pos) = ans;
            } else {
                assign->getRight()->accept(this);
                env[id] = pop();
            }
        }
        void visit(BinaryExpression* bin) override {
            bin->getLeft()->accept(this);
            bin->getRight()->accept(this);
            Object rhs = pop();
            Object lhs = pop();
            switch (bin->getToken().type) {
                case TK_PLUS:  push(add(lhs,rhs)); break;
                case TK_MINUS: push(sub(lhs,rhs)); break;
                case TK_MULT:  push(mul(lhs,rhs)); break;
                case TK_DIV:   push(div(lhs,rhs)); break;
                default:
                    break;
            }
        }
        void visit(RelOpExpression* rel) override {
            rel->getLeft()->accept(this);
            rel->getRight()->accept(this);
            Object rhs = pop();
            Object lhs = pop();
            switch (rel->getToken().type) {
                case TK_EQU: push(eq(lhs, rhs)); break;
                case TK_NEQ: push(neq(lhs,rhs)); break;
                case TK_LT: push(lt(lhs, rhs)); break;
                case TK_GT: push(gt(lhs, rhs)); break;
                case TK_LTE: push(lte(lhs, rhs)); break;
                case TK_GTE: push(gte(lhs, rhs)); break;
                default:
                    break;
            }
        }
        void visit(FuncDefStatement* ds) override {
            env[ds->getName()] = Object(new Function(ds->getName(), ds->getParams(), ds->getBody()));
        }
        void visit(ReturnStatement* rs) override {
            rs->getRetVal()->accept(this);
            bailout = true;
        }
        void visit(ParameterList* pl) override {
            
        }
        void visit(FunctionCall* fc) override {
            fc->getName()->accept(this);
            Function* func = pop().func;
            auto argsList = fc->getArgs();
            auto paramsList = func->paramList()->getParams();
            auto ait = argsList.begin();
            auto pit = paramsList.begin();
            openScope();
            while (ait != argsList.end() && pit != paramsList.end()) {
                string id = (dynamic_cast<VarDefStatement*>(*pit))->getName();
                (*ait)->accept(this);
                env.emplace(id,pop());
                ait++;
                pit++;
            }
            bailout = false;
            func->getBody()->accept(this);
            bailout = false;
            closeScope();
        }
        void visit(SubscriptExpression* se) override {
            se->getName()->accept(this);
            auto object = pop();
            se->getPosition()->accept(this);
            int position = pop().numval;
            auto result = object.vec->at(position);
            push(result);
        }
        void visit(ListExpression* le) override {
            vector<Object>* vec = new vector<Object>();
            for (auto m : le->getExprsList()) {
                m->accept(this);
                Object obj = pop();
                cout<<obj<<endl;
                vec->push_back(obj);
            }
            push(Object(vec));
        }
};

#endif