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
    Stmt* parse_condition_expr();
    Stmt* parse_while();
    Expr* parse_expr();
    Expr* parse_primary_expr();
    Expr* parse_additive_expr();
    Expr* parse_boolean_expr();
    Expr* parse_logical_expr();
    Expr* parse_mult_expr();
    Expr* parse_assignment_expr();
    Expr* parse_object_expr();
    Expr* parse_call_member_expr();
    Expr* parse_member_expr();
    Expr* parse_call_expr(Expr* call);
    vector <Expr*> parse_args();
    vector <Expr*> parse_args_list();

    public:

    Parser(Token* tokens){
        this->i = 0;
        this->tokens = tokens;
    }

    Program* produceAST();


};

#endif