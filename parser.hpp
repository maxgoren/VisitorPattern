#ifndef parser_hpp
#define parser_hpp
#include <iostream>
#include <vector>
#include <list>
#include "token.hpp"
#include "syntaxtree.hpp"
#include "visitors.hpp"
using namespace std;

/*

    <Program> := <statementList>
    <StatmentList> := <statement> ; {<statement>;}*
    <Statement> := [ <WhileStmt> | <IfStmt> | <PrintStmt> | <ExprStmt> ] ;
    <WhileStmt> := while ( <expression> ) { statementList }
    <IfStmt>    := if ( <expression> ) { statementList } else { statementList }
    <PrintStmt> := println <expression>
    <ExprStmt>  := <expression> ;
    <expression> := <relop> ':=' <relop>
    <relop>   := <term> ( == | != | < | > | <= | >= ) <term>
    <term>    := <factor> (+|-) <factor>
    <factor>  := <val> (*|/) <val>
    <unop>    := -<val>
    <val>     := id '(' argsList ')'
    <primary> := number | id | string | (<expr>)
    <argList> := <expression> { ',' <expression> }*
 */

class Parser {
    private:
        vector<Token> tokens;
        int tpos;
        void advance() {
            tpos++;
        }
        Token current() {
            if (tpos < tokens.size())
                return tokens[tpos];    
            return tokens.back();
        }
        bool expect(TokenType token) {
            return token == tokens[tpos].type;
        }
        bool match(TokenType token) {
            if (token == tokens[tpos].type) {
                advance();
                return true;
            }
            cout<<"Mismatched token near: "<<current().lexeme<<endl;
            return false;
        }
        void init(vector<Token>& tkns) {
            tokens = tkns;
            tpos = 0;
        }
        ExpressionNode* primary() {
            if (expect(TK_NUMBER)) {
                LiteralExpression* lit = new LiteralExpression(current());
                match(TK_NUMBER);
                return lit;
            } else if (expect(TK_ID)) {
                IdExpression* id = new IdExpression(current());
                match(TK_ID);
                return id;
            } else if (expect(TK_TRUE)) {
                LiteralExpression* be = new LiteralExpression(current());
                match(TK_TRUE);
                return be;
            } else if (expect(TK_FALSE)) {
                LiteralExpression* be = new LiteralExpression(current());
                match(TK_FALSE);
                return be;
            } else if (expect(TK_STRING)) {
                LiteralExpression* lit = new LiteralExpression(current());
                match(TK_STRING);
                return lit;
            } else if (expect(TK_LPAREN)) {
                match(TK_LPAREN);
                ExpressionNode* expr = expression();
                match(TK_RPAREN);
                return expr;
            } else if (expect(TK_LBRACK)) {
                ListExpression* le = new ListExpression(current());
                match(TK_LBRACK);
                le->setExprsList(argsList());
                match(TK_RBRACK);
                return le;
            }
            return nullptr;
        }
        ExpressionNode* val() {
            ExpressionNode* node = primary();
            while (expect(TK_LBRACK)) {
                SubscriptExpression* se = new SubscriptExpression(current());
                match (TK_LBRACK);
                se->setName(dynamic_cast<IdExpression*>(node));
                se->setPosition(expression());
                match(TK_RBRACK);
                node = se;
            }
            while (expect(TK_LPAREN)) {
                FunctionCall* m = new FunctionCall(current());
                match(TK_LPAREN);
                m->setName(dynamic_cast<IdExpression*>(node));
                m->setArgs(argsList());
                match(TK_RPAREN);
                node = m;
            }
            return node;
        }
        ExpressionNode* unaryop() {
            ExpressionNode* node;
            if (expect(TK_MINUS)) {
                UnaryExpression* node = new UnaryExpression(current());
                match(TK_MINUS);
                node->setLeft(unaryop());
                return node;
            }
            node = val();
            return node;
        }
        ExpressionNode* factor() {
            ExpressionNode* node = unaryop();
            while (expect(TK_MULT) || expect(TK_DIV)) {
                BinaryExpression* bin = new BinaryExpression(current());
                match(current().type);
                bin->setLeft(node);
                bin->setRight(unaryop());
                node = bin;
            }
            return node;
        }
        ExpressionNode* term() {
            ExpressionNode* node = factor();
            while (expect(TK_PLUS) || expect(TK_MINUS)) {
                BinaryExpression* bin = new BinaryExpression(current());
                match(current().type);
                bin->setLeft(node);
                bin->setRight(factor());
                node = bin;
            }
            return node;
        }
        ExpressionNode* relop() {
            ExpressionNode* node = term();
            while (isRelOp(current().type)) {
                RelOpExpression* tmp = new RelOpExpression(current());
                match(current().type);
                tmp->setLeft(node);
                tmp->setRight(term());
                node = tmp;
            }
            return node;
        }
        ExpressionNode* expression() {
            ExpressionNode* node = relop();
            if (expect(TK_ASSIGN)) {
                AssignExpression* bin = new AssignExpression(current());
                match(current().type);
                if (node->getToken().type == TK_RBRACK)
                    bin->setLeft((SubscriptExpression*)node);
                else bin->setLeft((IdExpression*)node);
                bin->setRight(relop());
                node = bin;
            }
            return node;
        }
        IfStatement* parseIf() {
            IfStatement* is = new IfStatement(current());
            match(TK_IF);
            match(TK_LPAREN);
            is->setTestExpr(expression());
            match(TK_RPAREN);
            match(TK_LCURLY);
            is->setPassCase(statementList());
            match(TK_RCURLY);
            if (expect(TK_ELSE)) {
                match(TK_ELSE);
                match(TK_LCURLY);
                is->setFailCase(statementList());
                match(TK_RCURLY);
            } else is->setFailCase(nullptr);
            return is;
        }
        WhileStatement* parseWhile() {
            WhileStatement* ws = new WhileStatement(current());
            match(TK_WHILE);
            match(TK_LPAREN);
            ws->setTestExpr(expression());
            match(TK_RPAREN);
            match(TK_LCURLY);
            ws->setLoopBody(statementList());
            match(TK_RCURLY);
            return ws;
        }
        FuncDefStatement* parseFuncDef() {
            FuncDefStatement* ds = new FuncDefStatement(current());
            match(TK_DEFINE);
            ds->setName(current().lexeme);
            match(TK_ID);
            match(TK_LPAREN);
            if (!expect(TK_RPAREN)) {
                ds->setParams(parameterList());
            }
            match(TK_RPAREN);
            match(TK_LCURLY);
            ds->setBody(statementList());
            return ds;
        }
        StatementNode* statement() {
            StatementNode* stmt = nullptr;
            switch (current().type) {
                case TK_IF: {
                    return parseIf();
                } break;
                case TK_WHILE: {
                    return parseWhile();
                } break;
                case TK_DEFINE: {
                    return parseFuncDef();
                } break;
                case TK_VAR: {
                    VarDefStatement* vd = new VarDefStatement(current());
                    match(TK_VAR);
                    vd->setName(current().lexeme);
                    vd->setExpr(expression());
                    return vd;
                } break;
                case TK_RETURN: {
                    ReturnStatement* rs = new ReturnStatement(current());
                    match(TK_RETURN);
                    rs->setRetVal(expression());
                    return rs;
                } break;
                case TK_PRINT: {
                    PrintStatement* ps = new PrintStatement(current());
                    match(current().type);
                    ps->setExpression(expression());
                    return ps;
                } break;
                case TK_EOF: return nullptr; break;
                default: {
                    auto expr = expression();
                    if (expr != nullptr) {
                        ExprStatement* stmt = new ExprStatement(current());
                        stmt->setExpr(expr);
                        return stmt;
                    }
                }
            }
            return stmt;
        }
        list<ExpressionNode*> argsList() {
            list<ExpressionNode*> args;
            args.push_back(expression());
            while (expect(TK_COMA) && !(expect(TK_RPAREN) || expect(TK_RBRACK))) {
                match(TK_COMA);                                       
                args.push_back(expression());
            }
            return args;
        }
        ParameterList* parameterList() {
            ParameterList* pl = new ParameterList(current());
            pl->add(statement());
            while (expect(TK_COMA) && !expect(TK_RPAREN)) {
                match(TK_COMA);
                pl->add(statement());
            }
            return pl;
        }
        StatementList* statementList() {
            auto tk = current();
            list<StatementNode*> stmts;
            StatementNode* stmt = statement();
            if (stmt != nullptr)
                stmts.push_back(stmt);
            while (!expect(TK_RCURLY) && !expect(TK_EOF)) {
                if (expect(TK_SEMI)) match(TK_SEMI);
                stmt = statement();
                if (stmt != nullptr) stmts.push_back(stmt);
            }
            return new StatementList(tk, stmts);
        }
    public:
        Parser() {

        }
        ProgramStatement* parse(vector<Token>& tokens) {
            init(tokens);
            ProgramStatement* ps = new ProgramStatement(current());
            ps->setProgram(statementList());
            return ps;
        }
};

#endif