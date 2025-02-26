#ifndef PARSER
#define PARSER

#include <ast.hpp>
#include <lexer.hpp>
#include <iostream>
#include <memory>

class Parser{
    private:

    Token* tokens;

    int i = 0; // iterator in tokens;

    Token at();
    void next();
    Token eat();
    Token expect(TokenType t, string err);

    shared_ptr<Stmt> parse_stmt();
    shared_ptr<Stmt> parse_var_declaration();
    shared_ptr<Expr> parse_expr();
    shared_ptr<Expr> parse_primary_expr();
    shared_ptr<Expr> parse_additive_expr();
    shared_ptr<Expr> parse_mult_expr();
    shared_ptr<Expr> parse_assignment_expr();
    shared_ptr<Expr> parse_object_expr();

    public:

    Parser(Token* tokens){
        this->i = 0;
        this->tokens = tokens;
    }

    Program* produceAST();


};

#endif