#ifndef PARSER
#define PARSER

#include <ast.hpp>
#include <lexer.hpp>
#include <iostream>

class Parser{
    private:

    Token* tokens;

    int i = 0; // iterator in tokens;

    Token at();
    void next();
    Token eat();
    Token expect(TokenType t, string err);

    Stmt* parse_stmt();
    Stmt* parse_var_declaration();
    Expr* parse_expr();
    Expr* parse_primary_expr();
    Expr* parse_additive_expr();
    Expr* parse_mult_expr();

    public:

    Parser(Token* tokens){
        this->i = 0;
        this->tokens = tokens;
    }

    Program* produceAST();


};

#endif