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

Stmt* Parser::parse_stmt(){
    if(at().type == TokenType::Let || at().type == TokenType::Const){
        return parse_var_declaration();
    }
    return parse_expr();
}

Stmt* Parser::parse_var_declaration(){
    bool const isConst = eat().type == TokenType::Const;
    string const identifier = expect(TokenType::Identifier, "Let or Const declared wrong").value;
    
    if(at().type == TokenType::SEMICOLON){
        eat();
        if(isConst){
            cout << "Parser Error: Constant dəyər yoxdur!\n";
            exit(0); // !!! Debug sistemi ile deyis
        }

        VarDeclaration* temp = new VarDeclaration;
        temp->constant = false;
        temp->identifier = identifier;
        NumericLiteral* zero = new NumericLiteral("0");
        temp->val = zero;
        return temp;
    }

    expect(TokenType::ASSIGN, "Let or Const declared without assign or semicolon");
    VarDeclaration* temp = new VarDeclaration;
    temp->constant = isConst;
    temp->identifier = identifier;
    temp->val = parse_expr();
    expect(TokenType::SEMICOLON, "Let or Const declared without semicolon");
    return temp;
}

Expr* Parser::parse_expr(){
    //return parse_primary_expr();
    //return parse_additive_expr();
    return parse_assignment_expr();
}

Expr* Parser::parse_assignment_expr(){
    //Expr* left = parse_additive_expr();
    Expr* left = parse_object_expr();

    if(at().type == TokenType::ASSIGN){
        eat();
        Expr* val = parse_assignment_expr();
        AssignExpr* temp = new AssignExpr;
        temp->assignexpr = left;
        temp->value = val;
        return temp;
    }

    return left;
}

Expr* Parser::parse_object_expr(){
    if(at().type != TokenType::LBRACK) return parse_logical_expr();
    
    eat().value;
    vector <PropertyLiteral*> v;
    
    while(at().type != TokenType::EndOfFile && at().type != TokenType::RBRACK){
        string key = expect(TokenType::Identifier, "ObjectLiteral key is not found").value;
        int j;
        for(j = i;tokens[j].value != "=";--j); // Daha yaxsi check sistemi lazimdi
        string varname = tokens[j-1].value;
        if(key == varname){
            cout << "Parser Error: name of key cannot be same as parent object";
            exit(0); // debug systemi ile deyis
        }
        if(at().type == TokenType::COMMA){
            eat();
            PropertyLiteral* temp = new PropertyLiteral;
            temp->key = key;
            temp->val = nullptr;
            v.push_back(temp);
            continue;
        }
        else if(at().type == TokenType::RBRACK){
            PropertyLiteral* temp = new PropertyLiteral;
            temp->key = key;
            temp->val = nullptr;
            v.push_back(temp);
            continue;
        }

        expect(TokenType::COLON, "ObjectLiteral colon is missing");
        Expr* value = parse_expr();

        PropertyLiteral* temp = new PropertyLiteral;
        temp->key = key;
        temp->val = value;
        v.push_back(temp);
        if(at().type == TokenType::RBRACK) continue;
        expect(TokenType::COMMA, "ObjectLiteral comma is missing");
    }

    expect(TokenType::RBRACK, "Bracket bağlanmayıb");
    ObjectLiteral* temp = new ObjectLiteral;
    temp->properties = v;
    return temp;
}

Expr* Parser::parse_additive_expr(){
    Expr* left = parse_mult_expr();
    
    while(at().value == "+" || at().value == "-"){
        string op = eat().value;
        Expr* right = parse_mult_expr();
        BinaryExpr* binop = new BinaryExpr;
        binop->left = left;
        binop->op = op;
        binop->right = right;
        left = binop;
    }

    return left;
}

Expr* Parser::parse_boolean_expr(){
    Expr* left = parse_additive_expr();
    
    while(at().value == ">" || at().value == "<" || at().value == ">=" || at().value == "<=" || 
          at().value == "==" || at().value == "!="){
        string op = eat().value;
        Expr* right = parse_additive_expr();
        BinaryExpr* binop = new BinaryExpr;
        binop->left = left;
        binop->op = op;
        binop->right = right;
        left = binop;
    }


    return left;
}

Expr* Parser::parse_logical_expr(){
    Expr* left = parse_boolean_expr();
    
    while(at().value == "&&" || at().value == "||"){
        string op = eat().value;
        Expr* right = parse_boolean_expr();
        BinaryExpr* binop = new BinaryExpr;
        binop->left = left;
        binop->op = op;
        binop->right = right;
        left = binop;
    }

    return left;
}

Expr* Parser::parse_mult_expr(){
    // Expr* left = parse_primary_expr();
    Expr* left = parse_call_member_expr();
    
    while(at().value == "*" || at().value == "/" || at().value == "%"){
        string op = eat().value;
        Expr* right = parse_call_member_expr();
        BinaryExpr* binop = new BinaryExpr;
        binop->left = left;
        binop->op = op;
        binop->right = right;
        left = binop;
    }

    return left;
}

Expr* Parser::parse_call_member_expr(){
    Expr* member = parse_member_expr();
    
    if(at().type == TokenType::LPAREN) return parse_call_expr(member);
    
    return member;
}

Expr* Parser::parse_member_expr(){
    Expr* object = parse_primary_expr();

    while(at().type == TokenType::DOT || at().type == TokenType::LBRACE){
        Token op = eat();
        Expr* property;
        bool isComputed;

        if(op.type == TokenType::DOT){
            isComputed = false;
            property = parse_primary_expr();

            if(property->getKind() != NodeType::IDENTIFIER){
                cout << "Parse Error: Noqte identifier olmadan islenenmez";
                exit(0); // !!! Debug systemi ile deyis
            }
        }
        else{
            isComputed = true;
            property = parse_expr();
            expect(TokenType::RBRACE, "Kvadrat mötərizə bağlanmayıb");
        }

        MemberExpr* temp = new MemberExpr;
        temp->computed = isComputed;
        temp->property = property;
        temp->object = object;
        delete object; //
        object = temp;
    }

    return object;
}

Expr* Parser::parse_call_expr(Expr* call){
    CallExpr* call_expr = new CallExpr;
    call_expr->callexpr = call;
    call_expr->args = parse_args();
    
    if(at().type == TokenType::LPAREN)
        call_expr = (CallExpr*)parse_call_expr(call_expr);

    return call_expr;
}

vector<Expr*> Parser::parse_args(){
    expect(TokenType::LPAREN, "Mötərizə açılmalı idi :')");
    vector <Expr*> v;
    if(at().type != TokenType::RPAREN) v = parse_args_list();
    expect(TokenType::RPAREN, "Mötərizə bağlanmalı idi :')");
    return v;
}

vector<Expr*> Parser::parse_args_list(){
    vector <Expr*> v;
    v.push_back(parse_assignment_expr());

    while(at().type != TokenType::EndOfFile && at().type == TokenType::COMMA){
        eat();
        v.push_back(parse_assignment_expr());
    }

    return v;
}

Expr* Parser::parse_primary_expr(){
    const TokenType tk = at().type;

    if(tk == TokenType::Identifier){
        Expr* temp = new Identifier(eat().value);
        return temp;
    }
    else if(tk == TokenType::NOT){
        eat();
        NotExpr* temp = new NotExpr;
        //temp->val = parse_expr();
        temp->val = new Identifier(eat().value);
        return temp;
    }
    else if(tk == TokenType::Number){
        Expr* temp = new NumericLiteral(eat().value);
        //temp->kind = NodeType::NUMERIC_L;
        //cout << (int)temp->kind << '\n'; DEBUG CODE
        return temp;
    }
    else if(tk == TokenType::String){
        Expr* temp = new StringLiteral(eat().value);
        return temp;
    }
    else if(tk == TokenType::LPAREN){
        eat();
        Expr* temp = parse_expr();
        expect(TokenType::RPAREN, "Mötərizə bağlanmayıb!");
        return temp;
    }
    else{
        std::cout << "Bilinmeyen xeta! : parse_pr_expr - " << at().value; // !!! asert ile deyis
        exit(0);
        return new Expr();
    }
}

Program* Parser::produceAST(){
    Program* program = new Program;
    while(tokens[i].type != TokenType::EndOfFile){
        program->body.push_back(parse_stmt());
        //++i; // temp
    }

    return program;
}
