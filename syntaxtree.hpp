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
class ParameterList;
class PrintStatement;
class WhileStatement;
class FuncDefStatement;
class ReturnStatement;
class VarDefStatement;
class IfStatement;
class ExprStatement;
class LiteralExpression;
class IdExpression; 
class BinaryExpression;
class UnaryExpression;
class ListExpression;
class SubscriptExpression;
class RelOpExpression;
class AssignExpression;
class FunctionCall;

//Abstract Visitor Interface
class Visitor {
    public:
        virtual void visit(PrintStatement* ps) = 0;
        virtual void visit(WhileStatement* ws) = 0;
        virtual void visit(ExprStatement* es) = 0;
        virtual void visit(ProgramStatement* ps) = 0;
        virtual void visit(StatementList* sl) = 0;
        virtual void visit(ParameterList* pl) = 0;
        virtual void visit(IfStatement* is) = 0;
        virtual void visit(FuncDefStatement* ds) = 0;
        virtual void visit(ReturnStatement* rs) = 0;
        virtual void visit(VarDefStatement* vd) = 0;
        virtual void visit(IdExpression* idexpr) = 0;
        virtual void visit(LiteralExpression* litexpr) = 0;
        virtual void visit(AssignExpression* assignExpr) = 0;
        virtual void visit(BinaryExpression* binexpr) = 0;
        virtual void visit(RelOpExpression* relexpr) = 0;
        virtual void visit(UnaryExpression* unaryexpr) = 0;
        virtual void visit(FunctionCall* func) = 0;
        virtual void visit(ListExpression* le) = 0;
        virtual void visit(SubscriptExpression* se) = 0;
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
        virtual ~ExpressionNode() { }
};

//Base Stmt Class
class StatementNode : public ASTNode {
    public:
        StatementNode(Token tok) : ASTNode(tok) { }
        virtual ~StatementNode() { }
};

class StatementList : StatementNode {
    private:
        list<StatementNode*> statements;
    public:
        StatementList(Token tk, list<StatementNode*> stmts) : StatementNode(tk), statements(stmts) { }
        list<StatementNode*>& getStatements() { return statements; }
        void addStatement(StatementNode* stmt) { statements.push_back(stmt); }
        void accept(Visitor* visit) { visit->visit(this); }
        ~StatementList() {
            for (StatementNode* stmt : statements) {
                delete stmt;
            }
        }
};

class ParameterList : public StatementNode {
    private:
        list<StatementNode*> params;
    public:
        ParameterList(Token token) : StatementNode(token) { }
        list<StatementNode*> getParams() { return params; }
        void setParams(list<StatementNode*>& p) { params = p; }
        void add(StatementNode* sn) { params.push_back(sn); }
        void accept(Visitor* visitor) { visitor->visit(this); }
};

class ProgramStatement : public StatementNode {
    private:
        StatementList* statementList;
    public:
        ProgramStatement(Token token) : StatementNode(token) { }
        void setProgram(StatementList* sn) { statementList = sn; }
        StatementList* getStatement() { return statementList; }
        void accept(Visitor* visitor) { visitor->visit(this); }
        ~ProgramStatement() {
            delete statementList;
        }
};

class PrintStatement : public StatementNode {
    private:
        ExpressionNode* expression;
    public:
        PrintStatement(Token token) : StatementNode(token) { }
        void setExpression(ExpressionNode* expr) { expression = expr; }
        ExpressionNode* getExpression() { return expression; }
        void accept(Visitor* visitor) { visitor->visit(this); }
        ~PrintStatement() {
            delete expression;
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
        void accept(Visitor* visitor) { visitor->visit(this); }
        ~WhileStatement() {
            delete testExpr;
            delete body;
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
        void accept(Visitor* visitor) { visitor->visit(this); }
        ~IfStatement() {
            delete testExpr;
            delete trCase;
            delete faCase;
        }
};

class VarDefStatement : public StatementNode {
    private:
        string varname;
        ExpressionNode* expr;
        bool initialized;
    public:
        VarDefStatement(Token tk) : StatementNode(tk) { }
        bool isInitialized() { return initialized; }
        void setInitialized(bool v) { initialized = v; }
        void setName(string n) { varname = n; }
        string getName() { return varname; }
        void setExpr(ExpressionNode* e) { expr = e; }
        ExpressionNode* getExpr() { return expr; }
        void accept(Visitor* visitor) { visitor->visit(this); }
};

class FuncDefStatement : public StatementNode {
    private:
        string name;
        ParameterList* params;
        StatementList* body;
    public:
        FuncDefStatement(Token tk) : StatementNode(tk) { }
        ParameterList* getParams() { return params; }
        StatementList* getBody() { return body; }
        string getName() { return name; }
        void setParams(ParameterList* sl) { params = sl; }
        void setBody(StatementList* sl) { body = sl; }
        void setName(string nm) { name = nm; }
        void accept(Visitor* visitor) { visitor->visit(this); }
        ~FuncDefStatement() {
            delete params;
            delete body;
        }
};

class ReturnStatement : public StatementNode {
    private:
        ExpressionNode* retVal;
    public:
        ReturnStatement(Token tk) : StatementNode(tk) { }
        void setRetVal(ExpressionNode* expr) { retVal = expr; }
        ExpressionNode* getRetVal() { return retVal; }
        void accept(Visitor* visitor) { visitor->visit(this); }
 };

class ExprStatement : public StatementNode {
    private:
        ExpressionNode* expr;
    public:
        ExprStatement(Token token) : StatementNode(token) { }
        void setExpr(ExpressionNode* expression) { expr = expression; }
        ExpressionNode* getExpression() { return expr; }
        void accept(Visitor* visitor) {visitor->visit(this); }
        ~ExprStatement() { delete expr; }
};

class IdExpression : public ExpressionNode {
    public:
        IdExpression(Token token) : ExpressionNode(token) { }
        string getId() { return getToken().lexeme; }
        void accept(Visitor* visitor) { visitor->visit(this); }
        ~IdExpression() { }
};

class LiteralExpression : public ExpressionNode {
    public:
        LiteralExpression(Token token) : ExpressionNode(token) { }
        Object eval(Environment& e) { 
            switch (getToken().type) {
                case TK_NUMBER: return Object(std::stod(getToken().lexeme));
                case TK_STRING: return Object(getToken().lexeme);
                case TK_TRUE:   return Object(true);
                case TK_FALSE:  return Object(false);
                default: break;
            }
            return Object();
        }
        void accept(Visitor* visitor) { visitor->visit(this); }
        ~LiteralExpression() { }
};

class ListExpression : public ExpressionNode {
    private:
        list<ExpressionNode*> exprs;
    public:
        ListExpression(Token tk) : ExpressionNode(tk) { }
        void setExprsList(list<ExpressionNode*> l) { exprs = l; }
        list<ExpressionNode*>& getExprsList() { return exprs; }
        void addExpr(ExpressionNode* e) { exprs.push_back(e); }
        void accept(Visitor* visitor) { visitor->visit(this); }
};

class SubscriptExpression : public ExpressionNode {
    private:
        IdExpression* name;
        ExpressionNode* position;
    public:
        SubscriptExpression(Token tk) : ExpressionNode(tk) { }
        void setName(IdExpression* expr) { name = expr; }
        IdExpression* getName() { return name; }
        void setPosition(ExpressionNode* expr) { position = expr; }
        ExpressionNode* getPosition() { return position; }
        void accept(Visitor* visit) { visit->visit(this); }
};

class UnaryExpression : public ExpressionNode {
    private:
        ExpressionNode* left;
    public:
        UnaryExpression(Token tk) : ExpressionNode(tk) { }
        void setLeft(ExpressionNode* expr) { left = expr; }
        ExpressionNode* getLeft() { return left; }
        void accept(Visitor* visitor) { visitor->visit(this); }
        ~UnaryExpression() {
            delete left;
        }
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
        ~BinaryExpression() {
            delete left;
            delete right;
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
        ~RelOpExpression() {
            delete left;
            delete right;
        }
};

class AssignExpression : public ExpressionNode {
    private:
        ExpressionNode* left;
        ExpressionNode* right;
    public:
        AssignExpression(Token tk) : ExpressionNode(tk) { }
        void setLeft(ExpressionNode* expr) { left = expr; }
        void setRight(ExpressionNode* expr) { right = expr; }
        ExpressionNode* getLeft() { return left; }
        ExpressionNode* getRight() { return right; }
        void accept(Visitor* visitor) { visitor->visit(this); }
        ~AssignExpression() {
            delete left;
            delete right;
        }
};

class FunctionCall : public ExpressionNode {
    private:
        IdExpression* name;
        list<ExpressionNode*> arguments;
    public:
        FunctionCall(Token tok) : ExpressionNode(tok) { }
        void setName(IdExpression* expr) { name = expr; }
        void setArgs(list<ExpressionNode*> args) { arguments = args; }
        IdExpression* getName() { return name; }
        list<ExpressionNode*>& getArgs() { return arguments; }
        void accept(Visitor* visitor) { visitor->visit(this); }
};


#endif