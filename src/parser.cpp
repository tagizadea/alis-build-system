#include <parser.hpp>

Token Parser::at(){
    return tokens[i];
}

Token Parser::eat(){
    return tokens[i+1];
}

Stmt Parser::parse_stmt(){
    return parse_expr();
}

Expr Parser::parse_expr(){
    return parse_primary_expr();
}

Expr Parser::parse_primary_expr(){
    const TokenType tk = at().type;

    switch(tk){
    case TokenType::Identifier :
        return Identifier(eat().value);
    
    case TokenType::Number:
        return NumericLiteral(eat().value);

    default:
        std::cout << "Bilinmeyen xeta! : parse_pr_expr"; // !!! asert ile deyis
    }
}

Program Parser::produceAST(){
    Program program;

    while(tokens[i].type != TokenType::EndOfFile){
        program.body.push_back(parse_stmt());
    }

    return program;
}
