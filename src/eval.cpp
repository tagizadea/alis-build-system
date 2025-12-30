#include <eval.hpp>
#include <cmath>

Value* eval_program(Program* program, Env* env){
    if(program->body.empty()) return env->lookUpVar("Null");
    
    Value* last;
    
    for(int i=0;i<program->body.size();++i){
        NodeType kind = program->body[i]->getKind();
        if(kind != NodeType::NUMERIC_L && kind != NodeType::STRING_L) 
            last = evaluate(program->body[i], env);
    }

    return last;
}

bool isInteger(long double num) {
    return num == std::floor(num);
}

Value* eval_bin_expr(BinaryExpr* binop, Env* env){
    Value* lhs = evaluate(binop->left, env);
    Value* rhs = evaluate(binop->right, env);

    bool id_l = binop->left->getKind() != NodeType::IDENTIFIER;
    bool id_r = binop->right->getKind() != NodeType::IDENTIFIER;

    if(lhs->getType() == ValueType::String && rhs->getType() == ValueType::String){
        if(binop->op == "+"){
            StringVal* temp = new StringVal;
            StringVal* nl = (StringVal*)lhs;
            StringVal* nr = (StringVal*)rhs;
            temp->val = nl->val + nr->val;
            if(id_l) delete lhs;
            if(id_r) delete rhs;
            return temp;
        }
    }

    if(lhs->getType() == ValueType::String && rhs->getType() == ValueType::Number && binop->op == "*"){
        StringVal* temp = new StringVal;
        StringVal* nl = (StringVal*)lhs;
        NumberVal* nr = (NumberVal*)rhs;
        string s = "";
        for(int i=0;i<nr->val;++i) s += nl->val;
        temp->val = s;
        if(id_l) delete lhs;
        if(id_r) delete rhs;
        return temp;
    }
    else if(lhs->getType() == ValueType::String && rhs->getType() == ValueType::Number && binop->op == "*"){
        StringVal* temp = new StringVal;
        NumberVal* nl = (NumberVal*)lhs;
        StringVal* nr = (StringVal*)rhs;
        string s = "";
        for(int i=0;i<nl->val;++i) s += nr->val;
        temp->val = s;
        if(id_l) delete lhs;
        if(id_r) delete rhs;
        return temp;
    }

    if(lhs->getType() == ValueType::Bool && rhs->getType() == ValueType::Bool){
        if(binop->op == "&&"){
            BoolValue* temp = new BoolValue;
            BoolValue* nl = (BoolValue*)lhs;
            BoolValue* nr = (BoolValue*)rhs;
            temp->val = nl->val && nr->val;
            if(id_l) delete lhs;
            if(id_r) delete rhs;
            return temp;
        }
        if(binop->op == "||"){
            BoolValue* temp = new BoolValue;
            BoolValue* nl = (BoolValue*)lhs;
            BoolValue* nr = (BoolValue*)rhs;
            temp->val = nl->val || nr->val;
            if(id_l) delete lhs;
            if(id_r) delete rhs;
            return temp;
        }
        if(binop->op == "=="){
            BoolValue* temp = new BoolValue;
            BoolValue* nl = (BoolValue*)lhs;
            BoolValue* nr = (BoolValue*)rhs;
            temp->val = nl->val == nr->val;
            if(id_l) delete lhs;
            if(id_r) delete rhs;
            return temp;
        }
        if(binop->op == "!="){
            BoolValue* temp = new BoolValue;
            BoolValue* nl = (BoolValue*)lhs;
            BoolValue* nr = (BoolValue*)rhs;
            temp->val = nl->val != nr->val;
            if(id_l) delete lhs;
            if(id_r) delete rhs;
            return temp;
        }
    }

    if(lhs->getType() == ValueType::Number && rhs->getType() == ValueType::Number){
        if(binop->op == "+"){
            NumberVal* temp = new NumberVal;
            NumberVal* nl = (NumberVal*)lhs;
            NumberVal* nr = (NumberVal*)rhs;
            temp->val = nl->val + nr->val;
            if(id_l) delete lhs;
            if(id_r) delete rhs;
            return temp;
        }
        if(binop->op == "-"){
            NumberVal* temp = new NumberVal;
            NumberVal* nl = (NumberVal*)lhs;
            NumberVal* nr = (NumberVal*)rhs;
            temp->val = nl->val - nr->val;
            if(id_l) delete lhs;
            if(id_r) delete rhs;
            return temp;
        }
        if(binop->op == "*"){
            NumberVal* temp = new NumberVal;
            NumberVal* nl = (NumberVal*)lhs;
            NumberVal* nr = (NumberVal*)rhs;
            temp->val = nl->val * nr->val;
            if(id_l) delete lhs;
            if(id_r) delete rhs;
            return temp;
        }
        if(binop->op == "/"){
            NumberVal* temp = new NumberVal;
            NumberVal* nl = (NumberVal*)lhs;
            NumberVal* nr = (NumberVal*)rhs;
            temp->val = nl->val / nr->val;
            if(id_l) delete lhs;
            if(id_r) delete rhs;
            return temp;
        }
        if(binop->op == "%"){
            NumberVal* temp = new NumberVal;
            NumberVal* nl = (NumberVal*)lhs;
            NumberVal* nr = (NumberVal*)rhs;
            if(!isInteger(nl->val) || !isInteger(nr->val)){
                cout << "Evaluation Error: Kesr ededlerin MOD-u tapilmir!";
                exit(0); // !!! Debug systemi ile deyis
            }
            long long left = nl->val;
            long long right = nr->val;
            temp->val = left % right;
            if(id_l) delete lhs;
            if(id_r) delete rhs;
            return temp;
        }
        if(binop->op == ">"){
            BoolValue* temp = new BoolValue;
            NumberVal* nl = (NumberVal*)lhs;
            NumberVal* nr = (NumberVal*)rhs;
            temp->val = nl->val > nr->val;
            if(id_l) delete lhs;
            if(id_r) delete rhs;
            return temp;
        }
        if(binop->op == "<"){
            BoolValue* temp = new BoolValue;
            NumberVal* nl = (NumberVal*)lhs;
            NumberVal* nr = (NumberVal*)rhs;
            temp->val = nl->val < nr->val;
            if(id_l) delete lhs;
            if(id_r) delete rhs;
            return temp;
        }
        if(binop->op == ">="){
            BoolValue* temp = new BoolValue;
            NumberVal* nl = (NumberVal*)lhs;
            NumberVal* nr = (NumberVal*)rhs;
            temp->val = nl->val >= nr->val;
            if(id_l) delete lhs;
            if(id_r) delete rhs;
            return temp;
        }
        if(binop->op == "<="){
            BoolValue* temp = new BoolValue;
            NumberVal* nl = (NumberVal*)lhs;
            NumberVal* nr = (NumberVal*)rhs;
            temp->val = nl->val <= nr->val;
            if(id_l) delete lhs;
            if(id_r) delete rhs;
            return temp;
        }
        if(binop->op == "=="){
            BoolValue* temp = new BoolValue;
            NumberVal* nl = (NumberVal*)lhs;
            NumberVal* nr = (NumberVal*)rhs;
            temp->val = nl->val == nr->val;
            if(id_l) delete lhs;
            if(id_r) delete rhs;
            return temp;
        }
        if(binop->op == "!="){
            BoolValue* temp = new BoolValue;
            NumberVal* nl = (NumberVal*)lhs;
            NumberVal* nr = (NumberVal*)rhs;
            temp->val = nl->val != nr->val;
            if(id_l) delete lhs;
            if(id_r) delete rhs;
            return temp;
        }
    }

    return env->lookUpVar("Null");
}

Value* eval_object_expr(ObjectLiteral* obj, Env* env){
    ObjectValue* object = new ObjectValue;

    for(const PropertyLiteral* i : obj->properties){

        Value* val = (i->val == nullptr) ? (env->lookUpVar(i->key)) : (evaluate(i->val, env));

        object->properties[i->key] = val;
    }
    return object;
}

Value* eval_call_expr(CallExpr* expr, Env* env){ // memory leak
    vector <Value*> args;
    
    for(Expr* i : expr->args){
        args.push_back(evaluate(i, env));
    }

    Value* fn = evaluate(expr->callexpr, env);

    if(fn->getType() == ValueType::NFUNC){
        Value* result;
        NativeFuncVal* nfn = (NativeFuncVal*)fn;
        if(nfn->list){
            for(Value* i : nfn->call.args) args.push_back(i);
        }
        result = nfn->call.funAddr(args, env);
        return result;
    }
    else if(fn->getType() == ValueType::FUNC){
        FunctionVal* func = (FunctionVal*)fn;
        if(args.size() != func->params.size()){
            cout << "Evaluation Error: Some arguments are missing for function named \"" << func->name << '\"';
            exit(0); // !!! debug systemi ile deyis
        }
        Env* scope = new Env;
        scope->parent = func->decEnv;
        for(int i = 0; i < func->params.size(); ++i){
            scope->declareVar(func->params[i], args[i], false);
        }
        if(func->body.empty()) return env->lookUpVar("Null");
        Value* ret;
        for(Stmt* i : func->body){
            ret = evaluate(i, scope);
        }
        if(ret->getType() != ValueType::FUNC) delete scope; /// funksiyaVal return eleyende problem olur
        return ret;
    }
    cout << "Evaluation Error: Cannot call value that is not a function";
    exit(0); // !!! Debug systemi ile deyis
    return nullptr;
}

Value* eval_ident(Identifier* idn, Env* env){
    Value* val = env->lookUpVar(idn->symbol);
    return val;
}

Value* eval_var_declaration(VarDeclaration* var_d, Env* env){
    if(var_d->val->getKind() == NodeType::BREAK || var_d->val->getKind() == NodeType::CONTINUE){
        cout << "Evaluation Error: Break or Continue cannot be assigned!";
        exit(0); // !!! Debug systemi ile deyis
    }
    Value* value = var_d->val ? (evaluate(var_d->val, env)) : (env->lookUpVar("Null"));
    return env->declareVar(var_d->identifier, value, var_d->constant);
}

Value* eval_var_assignment(AssignExpr* as, Env* env){
    if(as->assignexpr->getKind() != NodeType::IDENTIFIER){
        cout << "Evaluation Error: Assignmentdə sol identifier işlənməyib!";
        exit(0); // !!! Debug ile deyis
    }
    if(as->value->getKind() == NodeType::BREAK || as->value->getKind() == NodeType::CONTINUE){
        cout << "Evaluation Error: Assignmentdə sag break ve ya continue olanmaz!";
        exit(0); // !!! Debug ile deyis
    }
    string varname = ((Identifier*)(as->assignexpr))->symbol;
    return env->assignVar(varname, evaluate(as->value, env));
}

Value* eval_condition(CondExpr* cond, Env* env){
    Value* condition = evaluate(cond->condition, env);
    
    if(condition->getType() != ValueType::Bool){
        cout << "Evaluation Error: IF condition must be boolean value";
        exit(0); // !!! debug systemi ile deyis
    }

    BoolValue* temp = (BoolValue*)condition;
    Env* scope = new Env;
    scope->parent = env;
    if(temp->val){
        //result = evaluate(cond->ThenBranch, env);
        for(Stmt* i : cond->ThenBranch){
            Value* res;
            res = evaluate(i, scope);
            if(res->getType() == ValueType::Break || res->getType() == ValueType::Continue) return res;
        }
    }
    else{
        for(Stmt* i : cond->ElseBranch){
            Value* res;
            res = evaluate(i, scope);
            if(res->getType() == ValueType::Break || res->getType() == ValueType::Continue) return res;
        }
    }
    // else evaluate(cond->ElseBranch, scope);
    delete scope;
    return env->lookUpVar("Null");
}

bool loop = false;

Value* eval_while(WhileStmt* wh, Env* env){
    Value* condition = evaluate(wh->condition, env);
    loop = true;
    if(condition->getType() != ValueType::Bool){
        cout << "Evaluation Error: WHILE condition must be boolean value";
        exit(0); // !!! debug systemi ile deyis
    }

    BoolValue* temp = (BoolValue*)condition;
    Env* scope = new Env;
    scope->parent = env;
    while(temp->val){
        bool br = false;
        for(Stmt* i : wh->ThenBranch){
            Value* res;
            res = evaluate(i, scope);
            if(res->getType() == ValueType::Break){
                br = true;
                delete res;
                break;
            }
            if(res->getType() == ValueType::Continue){
                delete res;
                break;
            }
        }
        if(br) break;
        condition = evaluate(wh->condition, env); // MEMORY ISSUE ola biler
        temp = (BoolValue*)condition;
    }
    delete scope;
    loop = false;
    return env->lookUpVar("Null");
}

Value* eval_for(ForStmt* fr, Env* env){
    loop = true;
    Env* scope = new Env;
    scope->parent = env;

    Value* it = evaluate(fr->iterator_dec, scope);

    Value* condition = evaluate(fr->condition, scope);

    if(condition->getType() != ValueType::Bool){
        cout << "Evaluation Error: WHILE condition must be boolean value";
        exit(0); // !!! debug systemi ile deyis
    }

    BoolValue* temp = (BoolValue*)condition;

    while(temp->val){
        bool br = false;
        for(Stmt* i : fr->ThenBranch){
            Value* res;
            res = evaluate(i, scope);
            if(res->getType() == ValueType::Break){
                br = true;
                delete res;
                break;
            }
            if(res->getType() == ValueType::Continue){
                delete res;
                break;
            }
        }
        if(br) break;
        Value* operation = evaluate(fr->operation, scope);
        condition = evaluate(fr->condition, scope);
        temp = (BoolValue*)condition;
    }
    delete scope;
    loop = false;
    return env->lookUpVar("Null");
}

Value* eval_member_val_expr(MemberExpr* me, Env* env){
    if(me->property == nullptr) return env->lookUpVar("Null");
    
    Value* obj_v = evaluate(me->object, env);

    if(obj_v->getType() == ValueType::List){
        ListValue* list = (ListValue*)obj_v;
        if(me->computed){
            // NumericLiteral* i = (NumericLiteral*)me->property;
            Value* i = evaluate(me->property, env);
            if(i->getType() != ValueType::Number){
                cout << "Index should be numeric! INPUT: " << (int)i->getType();
                exit(0); // !!! debug sistemi ile deyis
            }
            try{
                return list->v.at( int(((NumberVal*)i)->val) );
            }
            catch(const std::exception& e){
                std::cerr << e.what() << '\n'; 
                exit(0); // !!! debug systemi ile deyis
            }
        }
        else{
            string name = ((Identifier*)me->property)->symbol;
            NativeFuncVal* tnf = ListVecNFuncs[0];
            if(name == "size") tnf = ListVecNFuncs[0];
            else if(name == "push") tnf = ListVecNFuncs[1];
            else if(name == "pop") tnf = ListVecNFuncs[2];
            else if(name == "sort") tnf = ListVecNFuncs[3];
            else{
                cout << "Unknown list function!";
                exit(0); // !!! debug systemi ile deyis
            }
            tnf->list = true;
            tnf->call.args.clear();
            tnf->call.args.push_back(list);
            tnf->call.env = env;
            return tnf;
        }
    }

    //cout << "NODETYPE: " << (int)me->property->getKind(); //
    if(me->property->getKind() == NodeType::IDENTIFIER){
        Identifier* key = (Identifier*)me->property;
        ObjectValue* obj = (ObjectValue*)obj_v;
        return obj->properties[key->symbol];
    }
    if(me->property->getKind() == NodeType::CALLEXPR){ // parserde ele bunu
        cout << "Evaluation Error: CallExpr objectdə çağırıla bilməz";
        exit(0);
    }

    return evaluate(me->property, env);
}

Value* eval_func_declaration(FunDeclaration* fn, Env* env){
    FunctionVal* fn_val = new FunctionVal;
    fn_val->name = fn->name;
    fn_val->params = fn->parameters;
    fn_val->decEnv = env;
    fn_val->body = fn->body;
    return env->declareVar(fn->name, fn_val, true);
}

Value* eval_list_expr(ListLiteral* l, Env* env){
    ListValue* list_val = new ListValue;
    vector <Value*> v(l->properties.size());
    for(int i=0;i<l->properties.size();++i){
        ElementLiteral* el = l->properties[i];
        v[el->key] = evaluate(el->val, env);

        int val_id = (int)v[i]->getType();
        if(list_val->mapTypeCounter[val_id] == 0) ++list_val->distinc_types;
        ++list_val->mapTypeCounter[val_id];
        if(list_val->distinc_types == 1) list_val->consist_of = v[i]->getType();
        else list_val->consist_of = ValueType::None;
    }
    list_val->v = v;
    return list_val;
}

Value* eval_unary_val_expr(UnaryExpr* l, Env* env){
    NumberVal* temp = (NumberVal*)(evaluate(l->identifier, env));
    if(l->left){
        if(l->plus) ++temp->val;
        else --temp->val;
    }
    else{
        NumberVal* result = new NumberVal;
        result->val = temp->val;

        if(l->plus) ++temp->val;
        else --temp->val;

        return result;
    }

    return temp;
}

// Burda mem_valsory leak var | Garbage collector ya da smart_pointers ya da custom check mexanizm olmalidi ki, deyer deyisene assign olmursa islenenden sonra sil
Value* evaluate(Stmt* astNode, Env* env){
    if(astNode->getKind() == NodeType::NUMERIC_L){ //
        NumericLiteral* childObj = (NumericLiteral*)astNode;
        NumberVal* temp = new NumberVal;
        temp->val = childObj->val;
        return temp;
    }
    else if(astNode->getKind() == NodeType::NOTEXPR){ //
        NotExpr* childObj = (NotExpr*)astNode;
        Value* val = evaluate(childObj->val, env);
        if(val->getType() != ValueType::Bool){
            cout << "Evaluation Error: Not operation yalnız bool dəyərlər ilə işlənir";
            exit(0); // !!! debug systemi ile deyis
        }
        BoolValue* temp = new BoolValue;
        temp->val = !((BoolValue*)val)->val;
        return temp;
    }
    else if(astNode->getKind() == NodeType::STRING_L){
        StringLiteral* childObj = (StringLiteral*)astNode;
        StringVal* temp = new StringVal;
        temp->val = childObj->val;
        return temp;
    }
    else if(astNode->getKind() == NodeType::IDENTIFIER){ // SAFE
        Identifier* childObj = (Identifier*)astNode;
        return eval_ident(childObj, env);
    }
    else if(astNode->getKind() == NodeType::OBJECT_L){
        ObjectLiteral* childObj = (ObjectLiteral*)astNode;
        return eval_object_expr(childObj, env);
    }
    else if(astNode->getKind() == NodeType::LIST_L){
        ListLiteral* childObj = (ListLiteral*)astNode;
        return eval_list_expr(childObj, env);
    }
    else if(astNode->getKind() == NodeType::CALLEXPR){ // SAFE
        CallExpr* childObj = (CallExpr*)astNode;
        return eval_call_expr(childObj, env);
    }
    else if(astNode->getKind() == NodeType::ASSIGNEXPR){ // SAFE
        AssignExpr* childObj = (AssignExpr*)astNode;
        return eval_var_assignment(childObj, env);
    }
    else if(astNode->getKind() == NodeType::BINARYEXPR){ // SAFE
        BinaryExpr* childObj = (BinaryExpr*)astNode;
        return eval_bin_expr(childObj, env);
    }
    else if(astNode->getKind() == NodeType::PROGRAM){ // PARTIAL
        Program* childObj = (Program*)astNode;
        return eval_program(childObj, env);
    }
    else if(astNode->getKind() == NodeType::VAR_D){ // SAFE
        VarDeclaration* childObj = (VarDeclaration*)astNode;
        return eval_var_declaration(childObj, env);
    }
    else if(astNode->getKind() == NodeType::FUN_D){ // SAFFE
        FunDeclaration* childObj = (FunDeclaration*)astNode;
        return eval_func_declaration(childObj, env);
    }
    else if(astNode->getKind() == NodeType::CONDEXPR){ // SAFE
        CondExpr* childObj = (CondExpr*)astNode;
        return eval_condition(childObj, env);
    }
    else if(astNode->getKind() == NodeType::WHILE_LOOP){ // SAFE
        WhileStmt* childObj = (WhileStmt*)astNode;
        return eval_while(childObj, env);
    }
    else if(astNode->getKind() == NodeType::FOR_LOOP){
        ForStmt* childObj = (ForStmt*)astNode;
        return eval_for(childObj, env);
    }
    else if(astNode->getKind() == NodeType::MEMBEREXPR){ // SAFE
        MemberExpr* childObj = (MemberExpr*)astNode;
        return eval_member_val_expr(childObj, env);
    }
    else if(astNode->getKind() == NodeType::UNARYEXPR){
        UnaryExpr* childObj = (UnaryExpr*)astNode;
        return eval_unary_val_expr(childObj, env);
    }
    else if(astNode->getKind() == NodeType::CONTINUE){ // SAFE
        if(!loop){
            cout << "Eval Error: Continue cannot be used without loop!\n"; // !!! assert ile evezle
            exit(0);
        }
        return new ContinueVal;
    }
    else if(astNode->getKind() == NodeType::BREAK){ // SAFE
        if(!loop){
            cout << "Eval Error: Break cannot be used without loop!\n"; // !!! assert ile evezle
            exit(0);
        }
        return new BreakVal;
    }
    else{
        cout << "Eval Error: Unknown type!\n"; // !!! assert ile evezle
        exit(0);
    }
}
