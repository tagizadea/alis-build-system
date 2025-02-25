#include <parser.hpp>

Token Parser::at(){
    return tokens[i];
}

Token Parser::eat(){
    ++i;
    return at();
}

Stmt* Parser::parse_stmt(){
    return parse_expr();
}

Expr* Parser::parse_expr(){
    return parse_primary_expr();
}

Expr* Parser::parse_primary_expr(){
    const TokenType tk = at().type;

    if(tk == TokenType::Identifier){
        Expr* temp = new Identifier(at().value);
        //temp->kind = NodeType::IDENTIFIER;
        return temp;
    }
    else if(tk == TokenType::Number){
        Expr* temp = new NumericLiteral(at().value);
        //temp->kind = NodeType::NUMERIC_L;
        //cout << (int)temp->kind << '\n'; DEBUG CODE
        return temp;
    }
    else{
        std::cout << "Bilinmeyen xeta! : parse_pr_expr"; // !!! asert ile deyis
        exit(0);
        return new Expr();
    }
}

Program* Parser::produceAST(){
    Program* program = new Program;
    while(tokens[i].type != TokenType::EndOfFile){
        program->body.push_back(parse_stmt());
        ++i; // temp
    }

    return program;
}
