#include <parser.hpp>
using namespace std;
#include <debug.hpp>

Token Parser::at(){
    return tokens[i];
}

void Parser::setErrorLocation(){
    debug::Debugger& dbg = debug::Debugger::getInstance();
    dbg.setScriptLine(tokens[i].line);
    dbg.setScriptCol(tokens[i].col);
}

Token Parser::eat(){
    setErrorLocation();
    if(tokens[i].type == TokenType::EndOfFile){
        ABS_FATAL(cat::Parser, "parser.eof_eat");
    }
    return tokens[i++];
}

Token Parser::expect(TokenType t, string err){
    setErrorLocation();
    Token tk = eat();

    if(tk.type != t){
        ABS_FATAL(cat::Parser, "parser.expect_error", err, tk.value, (int)t);
    }

    return tk;
}

unique_ptr<Stmt> Parser::parse_stmt(){
    if(at().type == TokenType::Let || at().type == TokenType::Const){
        return parse_var_declaration();
    }
    if(at().type == TokenType::IF) return parse_condition_expr();
    if(at().type == TokenType::WHILE) return parse_while();
    if(at().type == TokenType::FOR) return parse_for();
    if(at().type == TokenType::BREAK) return parse_break();
    if(at().type == TokenType::CONTINUE) return parse_continue();
    if(at().type == TokenType::FN) return parse_func_declaration();
    return parse_expr();
}

unique_ptr<Stmt> Parser::parse_break(){
    eat();
    return make_unique<BreakStmt>();
}

unique_ptr<Stmt> Parser::parse_continue(){
    eat();
    return make_unique<ContinueStmt>();
}



unique_ptr<Stmt> Parser::parse_var_declaration(){
    bool const isConst = eat().type == TokenType::Const;
    TokenType last = TokenType::Invalid;
    auto temp = make_unique<VarDeclaration>();
    do{
        string const identifier = expect(TokenType::Identifier, "Let or Const declared wrong").value;
        if(at().type == TokenType::SEMICOLON || at().type == TokenType::COMMA){
            if(isConst){
                setErrorLocation();
                ABS_FATAL(cat::Parser, "parser.const_no_value");
            }

            temp->constant = false;
            if(temp->vars.find(identifier) == temp->vars.end()) temp->vars[identifier] = make_unique<NumericLiteral>("0");
            else{
                setErrorLocation();
                ABS_FATAL(cat::Parser, "parser.duplicate_var");
            }
        }
        else{
            expect(TokenType::ASSIGN, "Let or Const declared without assign or semicolon");
            temp->constant = isConst;
            if(temp->vars.find(identifier) == temp->vars.end()) temp->vars[identifier] = parse_expr();
            else{
                setErrorLocation();
                ABS_FATAL(cat::Parser, "parser.duplicate_var");
            }
        }
        last = at().type;
    }
    while(eat().type == TokenType::COMMA);
    if(last != TokenType::SEMICOLON){
        setErrorLocation();
        ABS_FATAL(cat::Parser, "parser.missing_semicolon");
    }
    return temp;
}

unique_ptr<Stmt> Parser::parse_func_declaration(){
    eat();
    string name = expect(TokenType::Identifier, "Function name is missing").value;
    vector <unique_ptr<Expr>> args = parse_args();
    vector <string> params;

    for(auto& arg : args){
        if(arg->getKind() != NodeType::IDENTIFIER){
            setErrorLocation();
            ABS_FATAL(cat::Parser, "parser.param_not_identifier");
        }
        params.push_back(((Identifier*)arg.get())->symbol);
    }

    expect(TokenType::LBRACK, "Funksiya təyini üçün qarışıq mötərizə açılmayıb");
    vector <unique_ptr<Stmt>> body;

    while(at().type != TokenType::EndOfFile && at().type != TokenType::RBRACK){
        body.push_back(parse_stmt());
    }
    expect(TokenType::RBRACK, "Funksiya təyini üçün qarışıq mötərizə bağlanmayıb");
    auto temp = make_unique<FunDeclaration>();
    temp->body = move(body);
    temp->name = name;
    temp->parameters = params;
    return temp;
}

unique_ptr<Expr> Parser::parse_expr(){
    return parse_assignment_expr();
}

unique_ptr<Stmt> Parser::parse_condition_expr(){
    eat();
    expect(TokenType::LPAREN, "IF left parenthesis is missing");
    unique_ptr<Expr> condition = parse_expr();
    expect(TokenType::RPAREN, "IF right parenthesis is missing");
    expect(TokenType::LBRACK, "IF left bracket is missing");
    vector <unique_ptr<Stmt>> ThenBranch;
    vector <unique_ptr<Stmt>> ElseBranch;
    while(TokenType::EndOfFile != at().type && TokenType::RBRACK != at().type){
        ThenBranch.push_back(parse_stmt());
    }
    expect(TokenType::RBRACK, "IF right bracket is missing");
    if(at().type == TokenType::ELSE){
        eat();
        bool f = false;
        if(at().type == TokenType::LBRACK){
            eat();
            f = true;
        }

        if(at().type == TokenType::IF){
            ElseBranch.push_back( parse_condition_expr() );
        }
        else{
            while(TokenType::EndOfFile != at().type && TokenType::RBRACK != at().type){
                ElseBranch.push_back(parse_stmt());
            }
        }

        if(f) expect(TokenType::RBRACK, "Else ucun bracket baglanmalidir");
    }

    auto temp = make_unique<CondExpr>();
    temp->condition = move(condition);
    temp->ElseBranch = move(ElseBranch);
    temp->ThenBranch = move(ThenBranch);
    return temp;
}

unique_ptr<Stmt> Parser::parse_while(){
    eat();
    expect(TokenType::LPAREN, "WHILE left parenthesis is missing");
    unique_ptr<Expr> condition = parse_expr();
    expect(TokenType::RPAREN, "WHILE right parenthesis is missing");
    expect(TokenType::LBRACK, "WHILE left bracket is missing");
    vector <unique_ptr<Stmt>> ThenBranch;
    while(TokenType::EndOfFile != at().type && TokenType::RBRACK != at().type){
        ThenBranch.push_back(parse_stmt());
    }
    expect(TokenType::RBRACK, "WHILE right bracket is missing");
    auto temp = make_unique<WhileStmt>();
    temp->condition = move(condition);
    temp->ThenBranch = move(ThenBranch);
    return temp;
}

unique_ptr<Stmt> Parser::parse_for(){
    eat();
    expect(TokenType::LPAREN, "FOR left parenthesis is missing");
    unique_ptr<Stmt> it_dec = parse_stmt();

    if(it_dec->getKind() != NodeType::VAR_D) expect(TokenType::SEMICOLON, "FOR semicolon is missing");

    unique_ptr<Expr> condition = parse_expr();
    expect(TokenType::SEMICOLON, "FOR semicolon is missing");

    unique_ptr<Expr> operation = parse_expr();

    expect(TokenType::RPAREN, "FOR right parenthesis is missing");
    expect(TokenType::LBRACK, "FOR left bracket is missing");
    vector <unique_ptr<Stmt>> ThenBranch;
    while(TokenType::EndOfFile != at().type && TokenType::RBRACK != at().type){
        ThenBranch.push_back(parse_stmt());
    }
    expect(TokenType::RBRACK, "FOR right bracket is missing");
    auto temp = make_unique<ForStmt>();
    temp->iterator_dec = move(it_dec);
    temp->condition = move(condition);
    temp->operation = move(operation);
    temp->ThenBranch = move(ThenBranch);
    return temp;
}

unique_ptr<Expr> Parser::parse_assignment_expr(){
    unique_ptr<Expr> left = parse_object_expr();

    if(at().type == TokenType::ASSIGN){
        eat();
        unique_ptr<Expr> val = parse_assignment_expr();
        auto temp = make_unique<AssignExpr>();
        temp->assignexpr = move(left);
        temp->value = move(val);
        return temp;
    }

    if(at().type == TokenType::PLUS_ASSIGN){
        eat();
        unique_ptr<Expr> val = parse_assignment_expr();
        auto temp = make_unique<AssignExpr>();
        auto temp_b = make_unique<BinaryExpr>();

        temp_b->left = left->clone();
        temp_b->right = move(val);
        temp_b->op = "+";
        
        temp->assignexpr = move(left);
        temp->value = move(temp_b);
        
        return temp;
    }

    if(at().type == TokenType::MINUS_ASSIGN){
        eat();
        unique_ptr<Expr> val = parse_assignment_expr();
        auto temp = make_unique<AssignExpr>();
        auto temp_b = make_unique<BinaryExpr>();

        temp_b->left = left->clone();
        temp_b->right = move(val);
        temp_b->op = "-";
        
        temp->assignexpr = move(left);
        temp->value = move(temp_b);
        
        return temp;
    }

    if(at().type == TokenType::MULT_ASSIGN){
        eat();
        unique_ptr<Expr> val = parse_assignment_expr();
        auto temp = make_unique<AssignExpr>();
        auto temp_b = make_unique<BinaryExpr>();

        temp_b->left = left->clone();
        temp_b->right = move(val);
        temp_b->op = "*";
        
        temp->assignexpr = move(left);
        temp->value = move(temp_b);
        
        return temp;
    }

    if(at().type == TokenType::DIV_ASSIGN){
        eat();
        unique_ptr<Expr> val = parse_assignment_expr();
        auto temp = make_unique<AssignExpr>();
        auto temp_b = make_unique<BinaryExpr>();

        temp_b->left = left->clone();
        temp_b->right = move(val);
        temp_b->op = "/";
        
        temp->assignexpr = move(left);
        temp->value = move(temp_b);
        
        return temp;
    }

    if(at().type == TokenType::MOD_ASSIGN){
        eat();
        unique_ptr<Expr> val = parse_assignment_expr();
        auto temp = make_unique<AssignExpr>();
        auto temp_b = make_unique<BinaryExpr>();

        temp_b->left = left->clone();
        temp_b->right = move(val);
        temp_b->op = "%";
        
        temp->assignexpr = move(left);
        temp->value = move(temp_b);
        
        return temp;
    }

    return left;
}

unique_ptr<Expr> Parser::parse_list_expr(){
    if(at().type != TokenType::LBRACE) return parse_logical_expr();

    eat();
    vector <unique_ptr<ElementLiteral>> v;

    unsigned int key = 0;
    while(at().type != TokenType::EndOfFile && at().type != TokenType::RBRACE){
        unique_ptr<Expr> val = parse_expr();
        if(at().type == TokenType::COMMA){
            eat();
            auto temp = make_unique<ElementLiteral>();
            temp->key = key++;
            temp->val = move(val);
            v.push_back(move(temp));
            continue;
        }
        else if(at().type == TokenType::RBRACE){
            auto temp = make_unique<ElementLiteral>();
            temp->key = key++;
            temp->val = move(val);
            v.push_back(move(temp));
            continue;
        }
    }

    expect(TokenType::RBRACE, "Brace bağlanmayıb");
    auto temp = make_unique<ListLiteral>();
    temp->properties = move(v);
    return temp;
}

unique_ptr<Expr> Parser::parse_object_expr(){
    if(at().type != TokenType::LBRACK) return parse_list_expr();
    
    eat();
    vector <unique_ptr<PropertyLiteral>> v;
    
    while(at().type != TokenType::EndOfFile && at().type != TokenType::RBRACK){
        string key = expect(TokenType::Identifier, "ObjectLiteral key is not found").value;
        int j;
        for(j = i;tokens[j].value != "{";--j); // Eyni adli local deyisen sistemi lazimdir
        if(tokens[j-1].type == TokenType::ASSIGN){
            string varname = tokens[j-2].value;
            if(key == varname){
                setErrorLocation();
                ABS_FATAL(cat::Parser, "parser.key_same_as_object");
            }
        }
        if(at().type == TokenType::COMMA){
            eat();
            auto temp = make_unique<PropertyLiteral>();
            temp->key = key;
            temp->val = nullptr;
            v.push_back(move(temp));
            continue;
        }
        else if(at().type == TokenType::RBRACK){
            auto temp = make_unique<PropertyLiteral>();
            temp->key = key;
            temp->val = nullptr;
            v.push_back(move(temp));
            continue;
        }

        expect(TokenType::COLON, "ObjectLiteral colon is missing");
        unique_ptr<Expr> value = parse_expr();

        auto temp = make_unique<PropertyLiteral>();
        temp->key = key;
        temp->val = move(value);
        v.push_back(move(temp));
        if(at().type == TokenType::RBRACK) continue;
        expect(TokenType::COMMA, "ObjectLiteral comma is missing");
    }

    expect(TokenType::RBRACK, "Bracket bağlanmayıb");
    auto temp = make_unique<ObjectLiteral>();
    temp->properties = move(v);
    return temp;
}

unique_ptr<Expr> Parser::parse_additive_expr(){
    unique_ptr<Expr> left = parse_mult_expr();
    
    while(at().value == "+" || at().value == "-"){
        string op = eat().value;
        unique_ptr<Expr> right = parse_mult_expr();
        auto binop = make_unique<BinaryExpr>();
        binop->left = move(left);
        binop->op = op;
        binop->right = move(right);
        left = move(binop);
    }

    return left;
}

unique_ptr<Expr> Parser::parse_unary_expr(){
    if(at().type == TokenType::UNARY_PLUS || at().type == TokenType::UNARY_MINUS){
        auto temp = make_unique<UnaryExpr>();
        temp->left = true;
        temp->plus = eat().type == TokenType::UNARY_PLUS;
        unique_ptr<Expr> r = parse_primary_expr();
        if(r->getKind() != NodeType::IDENTIFIER){
            setErrorLocation();
            ABS_FATAL(cat::Parser, "parser.unary_needs_identifier");
        }
        temp->identifier = move(r);
        return temp;
    }
    
    unique_ptr<Expr> right = parse_primary_expr();
    if(at().type == TokenType::UNARY_PLUS || at().type == TokenType::UNARY_MINUS){
        auto temp = make_unique<UnaryExpr>();
        temp->left = false;
        temp->plus = eat().type == TokenType::UNARY_PLUS;
        if(right->getKind() != NodeType::IDENTIFIER){
            setErrorLocation();
            ABS_FATAL(cat::Parser, "parser.unary_needs_identifier");
        }
        temp->identifier = move(right);
        return temp;
    }
    
    return right;
}

unique_ptr<Expr> Parser::parse_boolean_expr(){
    unique_ptr<Expr> left = parse_additive_expr();
    
    while(at().value == ">" || at().value == "<" || at().value == ">=" || at().value == "<=" || 
          at().value == "==" || at().value == "!="){
        string op = eat().value;
        unique_ptr<Expr> right = parse_additive_expr();
        auto binop = make_unique<BinaryExpr>();
        binop->left = move(left);
        binop->op = op;
        binop->right = move(right);
        left = move(binop);
    }


    return left;
}

unique_ptr<Expr> Parser::parse_logical_expr(){
    unique_ptr<Expr> left = parse_boolean_expr();
    
    while(at().value == "&&" || at().value == "||"){
        string op = eat().value;
        unique_ptr<Expr> right = parse_boolean_expr();
        auto binop = make_unique<BinaryExpr>();
        binop->left = move(left);
        binop->op = op;
        binop->right = move(right);
        left = move(binop);
    }

    return left;
}

unique_ptr<Expr> Parser::parse_mult_expr(){
    unique_ptr<Expr> left = parse_call_member_expr();
    
    while(at().value == "*" || at().value == "/" || at().value == "%"){
        string op = eat().value;
        unique_ptr<Expr> right = parse_call_member_expr();
        auto binop = make_unique<BinaryExpr>();
        binop->left = move(left);
        binop->op = op;
        binop->right = move(right);
        left = move(binop);
    }

    return left;
}

unique_ptr<Expr> Parser::parse_call_member_expr(){
    unique_ptr<Expr> member = parse_member_expr();
    
    if(at().type == TokenType::LPAREN) return parse_call_expr(move(member));
    
    return member;
}

unique_ptr<Expr> Parser::parse_member_expr(){
    unique_ptr<Expr> object = parse_unary_expr();

    while(at().type == TokenType::DOT || at().type == TokenType::LBRACE){
        Token op = eat();
        unique_ptr<Expr> property;
        bool isComputed;

        if(op.type == TokenType::DOT){
            isComputed = false;
            property = parse_primary_expr();

            if(property->getKind() != NodeType::IDENTIFIER){
                setErrorLocation();
                ABS_FATAL(cat::Parser, "parser.dot_needs_identifier");
            }
        }
        else{
            isComputed = true;
            property = parse_expr();
            expect(TokenType::RBRACE, "Kvadrat mötərizə bağlanmayıb");
        }

        auto temp = make_unique<MemberExpr>();
        temp->computed = isComputed;
        temp->property = move(property);
        temp->object = move(object);
        object = move(temp);
    }

    return object;
}

unique_ptr<Expr> Parser::parse_call_expr(unique_ptr<Expr> call){
    auto call_expr = make_unique<CallExpr>();
    call_expr->callexpr = move(call);
    call_expr->args = parse_args();
    
    if(at().type == TokenType::LPAREN)
        call_expr = unique_ptr<CallExpr>(static_cast<CallExpr*>(parse_call_expr(move(call_expr)).release()));

    return call_expr;
}

vector<unique_ptr<Expr>> Parser::parse_args(){
    expect(TokenType::LPAREN, "Mötərizə açılmalı idi :')");
    vector <unique_ptr<Expr>> v;
    if(at().type != TokenType::RPAREN) v = parse_args_list();
    expect(TokenType::RPAREN, "Mötərizə bağlanmalı idi :')");
    return v;
}

vector<unique_ptr<Expr>> Parser::parse_args_list(){
    vector <unique_ptr<Expr>> v;
    v.push_back(parse_assignment_expr());

    while(at().type != TokenType::EndOfFile && at().type == TokenType::COMMA){
        eat();
        v.push_back(parse_assignment_expr());
    }

    return v;
}

unique_ptr<Expr> Parser::parse_primary_expr(){
    const TokenType tk = at().type;

    if(tk == TokenType::Identifier){
        return make_unique<Identifier>(eat().value);
    }
    else if(tk == TokenType::NOT){
        eat();
        auto temp = make_unique<NotExpr>();
        temp->val = make_unique<Identifier>(expect(TokenType::Identifier, "not identifiersiz yazilanmaz").value);
        return temp;
    }
    else if(tk == TokenType::Number){
        return make_unique<NumericLiteral>(eat().value);
    }
    else if(tk == TokenType::String){
        return make_unique<StringLiteral>(eat().value);
    }
    else if(tk == TokenType::LPAREN){
        eat();
        unique_ptr<Expr> temp = parse_expr();
        expect(TokenType::RPAREN, "Mötərizə bağlanmayıb!");
        return temp;
    }
    else if(tk == TokenType::MINUS){
        eat();
        unique_ptr<Expr> temp = parse_primary_expr();
        if(temp->getKind() != NodeType::NUMERIC_L){
            setErrorLocation();
            ABS_FATAL(cat::Parser, "parser.minus_without_number");
        }
        NumericLiteral* num = (NumericLiteral*)temp.get();
        num->val = -(num->val);
        return temp;
    }
    else if(tk == TokenType::PLUS){
        eat();
        unique_ptr<Expr> temp = parse_primary_expr();
        if(temp->getKind() != NodeType::NUMERIC_L){
            setErrorLocation();
            ABS_FATAL(cat::Parser, "parser.plus_without_number");
        }
        return temp;
    }
    else{
        setErrorLocation();
        ABS_FATAL(cat::Parser, "parser.unknown_expr", at().value);
        return nullptr;
    }
}

unique_ptr<Program> Parser::produceAST(){
    ABS_PROFILE_FUNC();
    auto program = make_unique<Program>();
    while(tokens[i].type != TokenType::EndOfFile){
        program->body.push_back(parse_stmt());
    }

    return program;
}