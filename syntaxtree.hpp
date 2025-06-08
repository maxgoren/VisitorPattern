#ifndef syntaxtree_hpp
#define syntaxtree_hpp
#include <iostream>
#include <unordered_map>
#include <memory>
#include "object.hpp"
using namespace std;

typedef unordered_map<string, Object> Environment;

class ASTNode;
class ExpressionNode;
class StatementNode;

class ProgramStatement;
class StatementList;
class PrintStatement;
class WhileStatement;
class DefStatement;
class IfStatement;
class ExprStatement;
class LiteralExpression;
class IdExpression; 
class BinaryExpression;
class UnaryExpression;
class RelOpExpression;
class AssignExpression;
class BooleanExpression;

//Abstract Visitor Interface
class Visitor {
    public:
        //virtual void visit(StatementNode* sn) = 0;
        virtual void visit(PrintStatement* ps) = 0;
        virtual void visit(WhileStatement* ws) = 0;
        virtual void visit(ExprStatement* es) = 0;
        virtual void visit(ProgramStatement* ps) = 0;
        virtual void visit(StatementList* sl) = 0;
        virtual void visit(IfStatement* is) = 0;
        virtual void visit(DefStatement* ds) = 0;
        //virtual void visit(ExpressionNode* expr) = 0;
        virtual void visit(IdExpression* idexpr) = 0;
        virtual void visit(LiteralExpression* litexpr) = 0;
        virtual void visit(AssignExpression* assignExpr) = 0;
        virtual void visit(BinaryExpression* binexpr) = 0;
        virtual void visit(RelOpExpression* relexpr) = 0;
        virtual void visit(BooleanExpression* boolexpr) = 0;
        virtual void visit(UnaryExpression* unaryexpr) = 0;
};

//Base AST Class
class ASTNode {
    private:
        Token token;
    public:
        ASTNode(Token tok) : token(tok) { }
        Token getToken() {
            return token;
        }
        virtual void accept(Visitor* visitor) = 0;
};

//Base Expr Class
class ExpressionNode : public ASTNode {
    public:
        ExpressionNode(Token tok) : ASTNode(tok) { }
};

//Base Stmt Class
class StatementNode : public ASTNode {
    public:
        StatementNode(Token tok) : ASTNode(tok) { }
};

class StatementList : StatementNode {
    private:
        list<StatementNode*> statements;
    public:
        StatementList(Token tk, list<StatementNode*> stmts) : StatementNode(tk), statements(stmts) { }
        list<StatementNode*>& getStatements() { return statements; }
        void addStatement(StatementNode* stmt) { statements.push_back(stmt); }
        void accept(Visitor* visit) { visit->visit(this); }
};


class ProgramStatement : public StatementNode {
    private:
        StatementList* statementList;
    public:
        ProgramStatement(Token token) : StatementNode(token) { }
        void setProgram(StatementList* sn) { statementList = sn; }
        StatementList* getStatement() { return statementList; }
        void accept(Visitor* visitor) {
            visitor->visit(this);
        }
};

class PrintStatement : public StatementNode {
    private:
        ExpressionNode* expression;
    public:
        PrintStatement(Token token) : StatementNode(token) { }
        void setExpression(ExpressionNode* expr) { expression = expr; }
        ExpressionNode* getExpression() { return expression; }
        void accept(Visitor* visitor) {
            visitor->visit(this);
        }
};

class WhileStatement : public StatementNode {
    private:
        ExpressionNode* testExpr;
        StatementList* body;
    public:
        WhileStatement(Token token) : StatementNode(token) { }
        void setTestExpr(ExpressionNode* expr) { testExpr = expr; }
        void setLoopBody(StatementList* stmt) { body = stmt; }
        ExpressionNode* getTestExpr() { return testExpr; }
        StatementList* getLoopBody() { return body; }
        void accept(Visitor* visitor) {
            visitor->visit(this);
        }
};

class IfStatement : public StatementNode {
    private:
        ExpressionNode* testExpr;
        StatementList* trCase;
        StatementList* faCase;
    public:
        IfStatement(Token token) : StatementNode(token) { }
        void setTestExpr(ExpressionNode* expr) { testExpr = expr; }
        void setPassCase(StatementList* sl) { trCase = sl; }
        void setFailCase(StatementList* sl) { faCase = sl; }
        StatementList* getPassCase() { return trCase; }
        StatementList* getFailCase() { return faCase; }
        ExpressionNode* getTest() { return testExpr; }
        void accept(Visitor* visitor) {
           visitor->visit(this);
        }
};
class DefStatement : public StatementNode {
    private:
        string name;
        StatementList* params;
        StatementList* body;
    public:
        DefStatement(Token tk) : StatementNode(tk) { }
        StatementList* getParams() { return params; }
        StatementList* getBody() { return body; }
        void setParams(StatementList* sl) { params = sl; }
        
};

class ExprStatement : public StatementNode {
    private:
        ExpressionNode* expr;
    public:
        ExprStatement(Token token) : StatementNode(token) { }
        void setExpr(ExpressionNode* expression) { expr = expression; }
        ExpressionNode* getExpression() { return expr; }
        void accept(Visitor* visitor) {
            visitor->visit(this);
        }
};

class IdExpression : public ExpressionNode {
    private:

    public:
        IdExpression(Token token) : ExpressionNode(token) { }
        string getId() { return getToken().lexeme; }
        void accept(Visitor* visitor) {
            visitor->visit(this);
        }
};

class LiteralExpression : public ExpressionNode {
    private:

    public:
        LiteralExpression(Token token) : ExpressionNode(token) { }
        Object eval(Environment& e) { 
            switch (getToken().type) {
                case TK_NUMBER: return Object(std::stod(getToken().lexeme));
                case TK_STRING: return Object(getToken().lexeme);
            }
        }
        void accept(Visitor* visitor) {
            visitor->visit(this);
        }
};

class BooleanExpression : public ExpressionNode {
    public:
        BooleanExpression(Token tk) : ExpressionNode(tk) { }
        bool eval(Environment& env) { return getToken().type == TK_TRUE ? true:false; }
        void accept(Visitor* visitor) {
            visitor->visit(this);
        }
};

class UnaryExpression : public ExpressionNode {
    private:
        ExpressionNode* left;
    public:
        UnaryExpression(Token tk) : ExpressionNode(tk) { }
        void setLeft(ExpressionNode* expr) { left = expr; }
        ExpressionNode* getLeft() { return left; }
        void accept(Visitor* visitor) { visitor->visit(this); }
};

class BinaryExpression : public ExpressionNode {
    private:
        ExpressionNode* left;
        ExpressionNode* right;
    public:
        BinaryExpression(Token token) : ExpressionNode(token) { }
        void setLeft(ExpressionNode* ll) { left = ll; }
        void setRight(ExpressionNode* rr) { right = rr; }
        ExpressionNode* getLeft() { return left; }
        ExpressionNode* getRight() { return right; }
        void accept(Visitor* visitor) {
            visitor->visit(this);
        }
};

class RelOpExpression : public ExpressionNode {
    private:
        ExpressionNode* left;
        ExpressionNode* right;
    public:
        RelOpExpression(Token token) : ExpressionNode(token) { }
        void setLeft(ExpressionNode* ll) { left = ll; }
        void setRight(ExpressionNode* rr) { right = rr; }
        ExpressionNode* getLeft() { return left; }
        ExpressionNode* getRight() { return right; }
        void accept(Visitor* visitor) {
            visitor->visit(this);
        }
};

class AssignExpression : public ExpressionNode {
    private:
        IdExpression* left;
        ExpressionNode* right;
    public:
        AssignExpression(Token tk) : ExpressionNode(tk) { }
        void setLeft(IdExpression* expr) { left = expr; }
        void setRight(ExpressionNode* expr) { right = expr; }
        IdExpression* getLeft() { return left; }
        ExpressionNode* getRight() { return right; }
        void accept(Visitor* visitor) {
            visitor->visit(this);
        }
};

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
                cout<<" ";
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
            enter("print statement");
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
            is->getFailCase()->accept(this);
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
            cout<<idexpr->getToken().lexeme<<endl;
            leave();
        }
        void visit(LiteralExpression* lit) override {
            enter("Literal Expression");
            cout<<lit->getToken().lexeme<<endl;
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
            cout<<bin->getToken().lexeme<<endl;
            bin->getLeft()->accept(this);
            bin->getRight()->accept(this);
            leave();
        }
        void visit(UnaryExpression* unary) override {
            enter("unary expr");
            cout<<unary->getToken().lexeme<<endl;
            unary->getLeft()->accept(this);
            leave();
        }
         void visit(RelOpExpression* rel) override {
            enter("Relop Expression");
            rel->getLeft()->accept(this);
            rel->getRight()->accept(this);
            leave();
        }
        void visit(BooleanExpression* boo) override {
            enter("Boolean Constant");
            cout<<boo->getToken().lexeme<<endl;
            leave();
        }
        void visit(DefStatement* ds) override {

        }
}; 

class Interpreter : public Visitor {
    private:
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
    public:
        void visit(StatementList* sl) override {
            for (auto m : sl->getStatements()) {
                m->accept(this);
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
                is->getFailCase()->accept(this);
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
            push(env[idexpr->getId()]);
        }
        void visit(LiteralExpression* lit) override {
            push(Object(lit->eval(env)));
        }
        void visit(AssignExpression* assign) override {
            string id = assign->getLeft()->getId();
            assign->getRight()->accept(this);
            env[id] = pop();
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
                case TK_GT: push(gt(rhs, lhs)); break;
                case TK_LTE: push(lte(lhs, rhs)); break;
                case TK_GTE: push(gte(rhs, lhs)); break;
                default:
                    break;
            }
        }
        void visit(BooleanExpression* boo) override {
            push(boo->eval(env));
        }
        void visit(DefStatement* ds) override {

        }
};


#endif