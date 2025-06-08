#ifndef token_hpp
#define token_hpp
#include <iostream>
using std::string;

enum TokenType {
    TK_NUMBER, TK_ID, TK_STRING, TK_ASSIGN, TK_LPAREN, TK_RPAREN, TK_LCURLY, TK_RCURLY,
    TK_PLUS, TK_MINUS, TK_MULT, TK_DIV, TK_EQU, TK_NEQ, TK_LT, TK_LTE, TK_GT, TK_GTE, 
    TK_PRINT, TK_WHILE, TK_IF, TK_ELSE, TK_DEFINE, TK_RETURN, TK_TRUE, TK_FALSE, TK_NOT, TK_SEMI,
    TK_NIL, TK_EOF, TK_ERR
};


struct Token {
    TokenType type;
    string lexeme;
    Token(TokenType tt = TK_NIL, string str = "nil") : type(tt), lexeme(str) { }
};

bool isRelOp(TokenType token) {
    switch (token) {
        case TK_EQU:
        case TK_NEQ:
        case TK_LT:
        case TK_GT:
        case TK_LTE:
        case TK_GTE: return true;
        default: break;
    }
    return false;
}


#endif