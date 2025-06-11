#ifndef lexer_hpp
#define lexer_hpp
#include <iostream>
#include <vector>
#include <unordered_map>
#include "token.hpp"
using namespace std;

class Lexer {
    private:
        unordered_map<string, Token> reserved;
        const static char eolChar = 0x7e;
        string input;
        int spos;
        vector<Token> tokens;
        char advance() {
            spos++;
            return get();
        }
        char get() {
            if (spos < 0 || spos >= input.size())
                return eolChar;
            return input[spos];
        }
        char rewind() {
            spos--;
            return get();
        }
        bool done() {
            return get() == eolChar;
        }
        Token extractNumber() {
            string lexeme;
            while (isdigit(get())) {
                lexeme.push_back(get());
                advance();
            }
            if (get() == '.' && isdigit(advance())) {
                lexeme.push_back('.');
            }
            while (isdigit(get())) {
                lexeme.push_back(get());
                advance();
            }
            return Token(TK_NUMBER, lexeme);
        }
        Token extractIdentifier() {
            string lexeme;
            while (!done()) {
                if (isalpha(get()) || isdigit(get()) || get() == '_') {
                    lexeme.push_back(get());
                    advance();
                } else break;
            }
            return checkReserved(lexeme);
        }
        Token extractString() {
            advance();
            string lexeme;
            while (!done()) {
                if (get() != '\"') lexeme.push_back(get());
                else break;
                advance();
            }
            return Token(TK_STRING, lexeme);
        }
        Token checkReserved(string lexeme) {
            if (reserved.find(lexeme) != reserved.end()) {
                return reserved[lexeme];
            }
            return Token(TK_ID, lexeme);
        }
        Token checkSpecials() {
            switch (get()) {
                case ':': 
                    if (advance() == '=') {
                        return Token(TK_ASSIGN, ":=");
                    }
                break;
                case '(': return Token(TK_LPAREN, "(");
                case ')': return Token(TK_RPAREN, ")");
                case '{': return Token(TK_LCURLY, "{");
                case '}': return Token(TK_RCURLY, "}");
                case '[': return Token(TK_LBRACK, "[");
                case ']': return Token(TK_RBRACK, "]");
                case '+': return Token(TK_PLUS, "+");
                case '-': return Token(TK_MINUS, "-");
                case '*': return Token(TK_MULT, "*");
                case '/': return Token(TK_DIV, "/");
                case ';': return Token(TK_SEMI, ";");
                case ',': return Token(TK_COMA, ",");
                case '<': {
                    advance();
                    if (get() == '=') {
                        return Token(TK_LTE, "<=");
                    }
                    rewind();
                    return Token(TK_LT, "<");
                } break;
                case '>': {
                    advance();
                    if (get() == '=') {
                        return Token(TK_GTE, ">=");
                    }
                    rewind();
                    return Token(TK_GT, ">");
                } break;
                case '=': {
                    advance();
                    if (get() == '=') {
                        return Token(TK_EQU, "==");
                    }
                    rewind();
                    return Token(TK_ERR, "err");
                } break;
                case '!': {
                    advance();
                    if (get() == '=') {
                        return Token(TK_NEQ, "!=");
                    }
                    rewind();
                    return Token(TK_NOT, "!");
                } break;
                default:
                    break;
            }
            return Token(TK_ERR, "error");
        }
        void skipWhiteSpace() {
            while (!done()) {
                if (get() == ' ' || get() == '\t' || get() == '\r' || get() == '\n')
                    advance();
                else break;
            }
        }
        void init(string line) {
            input = line;
            spos = 0;
            tokens.clear();
        }
    public:
        Lexer() {
            reserved["println"] = Token(TK_PRINT, "println");
            reserved["while"] = Token(TK_WHILE, "while");
            reserved["true"] = Token(TK_TRUE, "true");
            reserved["false"] = Token(TK_FALSE, "false");
            reserved["if"] = Token(TK_IF, "if");
            reserved["else"] = Token(TK_ELSE, "else");
            reserved["def"] = Token(TK_DEFINE, "def");
            reserved["return"] = Token(TK_RETURN, "return");
            reserved["var"] = Token(TK_VAR, "var");
        }
        vector<Token> lex(string line) {
            init(line);
            while (!done()) {
                skipWhiteSpace();
                if (isdigit(get())) tokens.push_back(extractNumber());
                else if (isalpha(get()) || get() == '_') { tokens.push_back(extractIdentifier()); }
                else if (get() == '\"') { tokens.push_back(extractString()); }
                else {
                    tokens.push_back(checkSpecials());
                    advance();
                }
            }
            tokens.push_back(Token(TK_EOF, "<eof>"));
            return tokens;
        }
};

#endif