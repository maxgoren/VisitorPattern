#include <iostream>
#include "parser.hpp"
#include "lexer.hpp"
using namespace std;

class ASTBuilder {
    private:
        Lexer lexer;
        Parser parser;
        bool loud;
    public:
        ASTBuilder(bool debug = true) {
            loud = debug;
        }
        ProgramStatement* buildAST(string input) {
            auto tokens = lexer.lex(input);
            if (loud) {
                for (auto m : tokens) {
                    cout<<"[ "<<tokenStr[m.type]<<", "<<m.lexeme<<" ]"<<endl;
                }
            }
            return parser.parse(tokens);
        }
};

void repl() {
    bool looping = true;
    ASTBuilder builder;
    PrintVisitor pv;
    InterpreterVisitor iv;
    while (looping) {
        cout<<" > ";
        string input;
        getline(cin, input);
        if (input == "quit") {
            looping = false;
        } else {
            auto ast = builder.buildAST(input);
            pv.visit(ast);
            iv.visit(ast);
        }
    }
}


int main() {
    repl();
}