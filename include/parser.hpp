#ifndef PARSER
#define PARSER

#include <ast.hpp>
#include <lexer.hpp>
#include <iostream>
#include <memory>

class Parser{
    private:

    const std::vector<Token>& tokens;

    int i = 0; // iterator in tokens;

    Token at();
    void next();
    Token eat();
    Token expect(TokenType t, std::string err);
    void setErrorLocation();

    std::unique_ptr<Stmt> parse_stmt();
    std::unique_ptr<Stmt> parse_break();
    std::unique_ptr<Stmt> parse_continue();
    std::unique_ptr<Stmt> parse_var_declaration();
    std::unique_ptr<Stmt> parse_func_declaration();
    std::unique_ptr<Stmt> parse_condition_expr();
    std::unique_ptr<Stmt> parse_while();
    std::unique_ptr<Stmt> parse_for();
    std::unique_ptr<Expr> parse_expr();
    std::unique_ptr<Expr> parse_primary_expr();
    std::unique_ptr<Expr> parse_additive_expr();
    std::unique_ptr<Expr> parse_unary_expr();
    std::unique_ptr<Expr> parse_boolean_expr();
    std::unique_ptr<Expr> parse_logical_expr();
    std::unique_ptr<Expr> parse_mult_expr();
    std::unique_ptr<Expr> parse_assignment_expr();
    std::unique_ptr<Expr> parse_list_expr();
    std::unique_ptr<Expr> parse_object_expr();
    std::unique_ptr<Expr> parse_call_member_expr();
    std::unique_ptr<Expr> parse_member_expr();
    std::unique_ptr<Expr> parse_call_expr(std::unique_ptr<Expr> call);
    std::vector <std::unique_ptr<Expr>> parse_args();
    std::vector <std::unique_ptr<Expr>> parse_args_list();

    public:

    Parser(const std::vector<Token>& tokens) : tokens(tokens){
        this->i = 0;
    }

    std::unique_ptr<Program> produceAST();


};

#endif