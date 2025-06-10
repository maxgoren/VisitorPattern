#include <iostream>
#include "parser.hpp"
#include "lexer.hpp"
using namespace std;

void repl() {
    bool looping = true;
    Lexer lexer;
    Parser parser;
    Environment env;
    PrintVisitor pv;
    InterpreterVisitor iv;
    while (looping) {
        cout<<" > ";
        string input;
        getline(cin, input);
        vector<Token> tokens = lexer.lex(input);
        for (auto m : tokens) {
            cout<<"[ "<<tokenStr[m.type]<<", "<<m.lexeme<<" ]"<<endl;
        }
        auto t = parser.parse(tokens);
        pv.visit(t);
        iv.visit(t);
    }
}


int main() {
    repl();
}