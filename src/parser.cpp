#include <parser.hpp>

Token Parser::at(){
    return tokens[i];
}

Token Parser::eat(){
    return tokens[i++];
}

Token Parser::expect(TokenType t, string err){
    Token tk = eat();

    if(tk.type != t){
        cout << "Parser Error: " << err << " - Expecting: " << (int)t <<'\n';
        exit(0);
    }

    return tk;
}

shared_ptr<Stmt> Parser::parse_stmt(){
    if(at().type == TokenType::Let || at().type == TokenType::Const){
        return parse_var_declaration();
    }
    return parse_expr();
}

shared_ptr<Stmt> Parser::parse_var_declaration(){
    bool const isConst = eat().type == TokenType::Const;
    string const identifier = expect(TokenType::Identifier, "Let or Const declared wrong").value;
    
    if(at().type == TokenType::SEMICOLON){
        eat();
        if(isConst){
            cout << "Parser Error: Constant dəyər yoxdur!\n";
            exit(0); // !!! Debug sistemi ile deyis
        }

        shared_ptr<VarDeclaration> temp = make_shared<VarDeclaration>();
        temp->constant = false;
        temp->identifier = identifier;
        temp->val = 0;
        return temp;
    }

    expect(TokenType::ASSIGN, "Let or Const declared without assign or semicolon");
    shared_ptr<VarDeclaration> temp = make_shared<VarDeclaration>();
    temp->constant = isConst;
    temp->identifier = identifier;
    temp->val = parse_expr();
    expect(TokenType::SEMICOLON, "Let or Const declared without semicolon");
    return temp;
}

shared_ptr<Expr> Parser::parse_expr(){
    //return parse_primary_expr();
    //return parse_additive_expr();
    return parse_assignment_expr();
}

shared_ptr<Expr> Parser::parse_assignment_expr(){
    //Expr* left = parse_additive_expr();
    shared_ptr<Expr> left = parse_object_expr();

    if(at().type == TokenType::ASSIGN){
        eat();
        shared_ptr<Expr> val = parse_assignment_expr();
        shared_ptr<AssignExpr> temp = make_shared<AssignExpr>();
        temp->assignexpr = left;
        temp->value = val;
        return temp;
    }

    return left;
}

shared_ptr<Expr> Parser::parse_object_expr(){
    if(at().type != TokenType::LBRACK) return parse_additive_expr();
    
    eat();
    vector <shared_ptr<PropertyLiteral>> v;
    
    while(at().type != TokenType::EndOfFile && at().type != TokenType::RBRACK){
        string key = expect(TokenType::Identifier, "ObjectLiteral key is not found").value;
        
        if(at().type == TokenType::COMMA){
            eat();
            shared_ptr<PropertyLiteral> temp = make_shared<PropertyLiteral>();
            temp->key = key;
            temp->val = nullptr;
            v.push_back(temp);
            continue;
        }
        else if(at().type == TokenType::RBRACK){
            shared_ptr<PropertyLiteral> temp = make_shared<PropertyLiteral>();
            temp->key = key;
            temp->val = nullptr;
            v.push_back(temp);
            continue;
        }

        expect(TokenType::COLON, "ObjectLiteral colon is missing");
        shared_ptr<Expr> value = parse_expr();

        shared_ptr<PropertyLiteral> temp = make_shared<PropertyLiteral>();
        temp->key = key;
        temp->val = value;
        v.push_back(temp);
        if(at().type == TokenType::RBRACK) continue;
        expect(TokenType::COMMA, "ObjectLiteral comma is missing");
    }

    expect(TokenType::RBRACK, "Bracket bağlanmayıb");
    shared_ptr<ObjectLiteral> temp = make_shared<ObjectLiteral>();
    temp->properties = v;
    return temp;
}

shared_ptr<Expr> Parser::parse_additive_expr(){
    shared_ptr<Expr> left = parse_mult_expr();
    
    while(at().value == "+" || at().value == "-"){
        string op = eat().value;
        shared_ptr<Expr> right = parse_mult_expr();
        shared_ptr<BinaryExpr> binop = make_shared<BinaryExpr>();
        binop->left = left;
        binop->op = op;
        binop->right = right;
        left = binop;
    }

    return left;
}

shared_ptr<Expr> Parser::parse_mult_expr(){
    shared_ptr<Expr> left = parse_primary_expr();
    
    while(at().value == "*" || at().value == "/" || at().value == "%"){
        string op = eat().value;
        shared_ptr<Expr> right = parse_primary_expr();
        shared_ptr<BinaryExpr> binop = make_shared<BinaryExpr>();
        binop->left = left;
        binop->op = op;
        binop->right = right;
        left = binop;
    }

    return left;
}

shared_ptr<Expr> Parser::parse_primary_expr(){
    const TokenType tk = at().type;

    if(tk == TokenType::Identifier){
        shared_ptr<Expr> temp = make_shared<Identifier>(eat().value);
        return temp;
    }
    else if(tk == TokenType::Number){
        shared_ptr<Expr> temp = make_shared<NumericLiteral>(eat().value);
        //temp->kind = NodeType::NUMERIC_L;
        //cout << (int)temp->kind << '\n'; DEBUG CODE
        return temp;
    }
    else if(tk == TokenType::LPAREN){
        eat();
        shared_ptr<Expr> temp = parse_expr();
        expect(TokenType::RPAREN, "Mötərizə bağlanmayıb!");
        return temp;
    }
    else{
        std::cout << "Bilinmeyen xeta! : parse_pr_expr"; // !!! asert ile deyis
        exit(0);
        return make_shared<Expr>();
    }
}

Program* Parser::produceAST(){
    unique_ptr<Program> program = make_unique<Program>();
    while(tokens[i].type != TokenType::EndOfFile){
        program->body.push_back(parse_stmt());
        //++i; // temp
    }

    return program.get();
}
