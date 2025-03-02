#include <eval.hpp>
#include <cmath>

Value* eval_program(Program* program, Env* env){
    Value* last = new NullVal;

    for(int i=0;i<program->body.size();++i)
        last = evaluate(program->body[i], env);

    return last;
}

bool isInteger(long double num) {
    return num == std::floor(num);
}

Value* eval_bin_expr(BinaryExpr* binop, Env* env){
    Value* lhs = evaluate(binop->left, env);
    Value* rhs = evaluate(binop->right, env);

    if(lhs->getType() == ValueType::String && rhs->getType() == ValueType::String){
        if(binop->op == "+"){
            StringVal* temp = new StringVal;
            StringVal* nl = (StringVal*)lhs;
            StringVal* nr = (StringVal*)rhs;
            temp->val = nl->val + nr->val;
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
        return temp;
    }
    else if(lhs->getType() == ValueType::String && rhs->getType() == ValueType::Number && binop->op == "*"){
        StringVal* temp = new StringVal;
        NumberVal* nl = (NumberVal*)lhs;
        StringVal* nr = (StringVal*)rhs;
        string s = "";
        for(int i=0;i<nl->val;++i) s += nr->val;
        temp->val = s;
        return temp;
    }

    if(lhs->getType() == ValueType::Bool && rhs->getType() == ValueType::Bool){
        if(binop->op == "&&"){
            BoolValue* temp = new BoolValue;
            BoolValue* nl = (BoolValue*)lhs;
            BoolValue* nr = (BoolValue*)rhs;
            temp->val = nl->val && nr->val;
            return temp;
        }
        if(binop->op == "||"){
            BoolValue* temp = new BoolValue;
            BoolValue* nl = (BoolValue*)lhs;
            BoolValue* nr = (BoolValue*)rhs;
            temp->val = nl->val || nr->val;
            return temp;
        }
        if(binop->op == "=="){
            BoolValue* temp = new BoolValue;
            BoolValue* nl = (BoolValue*)lhs;
            BoolValue* nr = (BoolValue*)rhs;
            temp->val = nl->val == nr->val;
            return temp;
        }
        if(binop->op == "!="){
            BoolValue* temp = new BoolValue;
            BoolValue* nl = (BoolValue*)lhs;
            BoolValue* nr = (BoolValue*)rhs;
            temp->val = nl->val != nr->val;
            return temp;
        }
    }

    if(lhs->getType() == ValueType::Number && rhs->getType() == ValueType::Number){
        if(binop->op == "+"){
            NumberVal* temp = new NumberVal;
            NumberVal* nl = (NumberVal*)lhs;
            NumberVal* nr = (NumberVal*)rhs;
            temp->val = nl->val + nr->val;
            return temp;
        }
        if(binop->op == "-"){
            NumberVal* temp = new NumberVal;
            NumberVal* nl = (NumberVal*)lhs;
            NumberVal* nr = (NumberVal*)rhs;
            temp->val = nl->val - nr->val;
            return temp;
        }
        if(binop->op == "*"){
            NumberVal* temp = new NumberVal;
            NumberVal* nl = (NumberVal*)lhs;
            NumberVal* nr = (NumberVal*)rhs;
            temp->val = nl->val * nr->val;
            return temp;
        }
        if(binop->op == "/"){
            NumberVal* temp = new NumberVal;
            NumberVal* nl = (NumberVal*)lhs;
            NumberVal* nr = (NumberVal*)rhs;
            temp->val = nl->val / nr->val;
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
            return temp;
        }
        if(binop->op == ">"){
            BoolValue* temp = new BoolValue;
            NumberVal* nl = (NumberVal*)lhs;
            NumberVal* nr = (NumberVal*)rhs;
            temp->val = nl->val > nr->val;
            return temp;
        }
        if(binop->op == "<"){
            BoolValue* temp = new BoolValue;
            NumberVal* nl = (NumberVal*)lhs;
            NumberVal* nr = (NumberVal*)rhs;
            temp->val = nl->val < nr->val;
            return temp;
        }
        if(binop->op == ">="){
            BoolValue* temp = new BoolValue;
            NumberVal* nl = (NumberVal*)lhs;
            NumberVal* nr = (NumberVal*)rhs;
            temp->val = nl->val >= nr->val;
            return temp;
        }
        if(binop->op == "<="){
            BoolValue* temp = new BoolValue;
            NumberVal* nl = (NumberVal*)lhs;
            NumberVal* nr = (NumberVal*)rhs;
            temp->val = nl->val <= nr->val;
            return temp;
        }
        if(binop->op == "=="){
            BoolValue* temp = new BoolValue;
            NumberVal* nl = (NumberVal*)lhs;
            NumberVal* nr = (NumberVal*)rhs;
            temp->val = nl->val == nr->val;
            return temp;
        }
        if(binop->op == "!="){
            BoolValue* temp = new BoolValue;
            NumberVal* nl = (NumberVal*)lhs;
            NumberVal* nr = (NumberVal*)rhs;
            temp->val = nl->val != nr->val;
            return temp;
        }
    }

    return new NullVal;
}

Value* eval_object_expr(ObjectLiteral* obj, Env* env){
    ObjectValue* object = new ObjectValue;

    for(const PropertyLiteral* i : obj->properties){

        Value* val = (i->val == nullptr) ? (env->lookUpVar(i->key)) : (evaluate(i->val, env));

        object->properties[i->key] = val;
    }

    return object;
}

Value* eval_call_expr(CallExpr* expr, Env* env){
    vector <Value*> args;
    
    for(Expr* i : expr->args) args.push_back(evaluate(i, env));

    Value* fn = evaluate(expr->callexpr, env);

    if(fn->getType() == ValueType::NFUNC){
        Value* result = ((NativeFuncVal*)fn)->call.funAddr(args, env);
        return result;
    }
    else if(fn->getType() == ValueType::FUNC){
        FunctionVal* func = (FunctionVal*)fn;
        Env* scope = new Env;
        scope->parent = func->decEnv;
        if(args.size() != func->params.size()){
            cout << "Evaluation Error: Some arguments are missing for function named \"" << func->name << '\"';
            exit(0); // !!! debug systemi ile deyis
        }
        for(int i = 0; i < func->params.size(); ++i){
            scope->declareVar(func->params[i], args[i], false);
        }
        if(func->body.empty()) return Make_Null();
        Value* ret;
        for(Stmt* i : func->body){
            ret = evaluate(i, scope);
        }
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
    Value* value = var_d->val ? (evaluate(var_d->val, env)) : (Make_Null());
    return env->declareVar(var_d->identifier, value, var_d->constant);
}

Value* eval_var_assignment(AssignExpr* as, Env* env){
    if(as->assignexpr->getKind() != NodeType::IDENTIFIER){
        cout << "Evaluation Error: Assignmentdə sol identifier işlənməyib!";
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
    Value* result;

    if(temp->val){
        //result = evaluate(cond->ThenBranch, env);
        for(Stmt* i : cond->ThenBranch){
            result = evaluate(i, env);
        }
    }
    else result = evaluate(cond->ElseBranch, env);

    return result;
}

Value* eval_while(WhileStmt* wh, Env* env){
    Value* condition = evaluate(wh->condition, env);
    
    if(condition->getType() != ValueType::Bool){
        cout << "Evaluation Error: WHILE condition must be boolean value";
        exit(0); // !!! debug systemi ile deyis
    }

    BoolValue* temp = (BoolValue*)condition;
    Value* result;

    while(temp->val){
        for(Stmt* i : wh->ThenBranch){
            result = evaluate(i, env);
        }
        condition = evaluate(wh->condition, env);
        temp = (BoolValue*)condition;
    }
    return result;
}

Value* eval_member_expr(MemberExpr* me, Env* env){
    if(me->property == nullptr) return Make_Null();

    //cout << "NODETYPE: " << (int)me->property->getKind(); //
    if(me->property->getKind() == NodeType::IDENTIFIER){
        Value* obj_v = evaluate(me->object, env);
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

// Burda memory leak var | Garbage collector ya da smart_pointers ya da custom check mexanizm olmalidi ki, deyer deyisene assign olmursa islenenden sonra sil
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
    else if(astNode->getKind() == NodeType::IDENTIFIER){
        Identifier* childObj = (Identifier*)astNode;
        return eval_ident(childObj, env);
    }
    else if(astNode->getKind() == NodeType::OBJECT_L){
        ObjectLiteral* childObj = (ObjectLiteral*)astNode;
        return eval_object_expr(childObj, env);
    }
    else if(astNode->getKind() == NodeType::CALLEXPR){
        CallExpr* childObj = (CallExpr*)astNode;
        return eval_call_expr(childObj, env);
    }
    else if(astNode->getKind() == NodeType::ASSIGNEXPR){
        AssignExpr* childObj = (AssignExpr*)astNode;
        return eval_var_assignment(childObj, env);
    }
    else if(astNode->getKind() == NodeType::BINARYEXPR){
        BinaryExpr* childObj = (BinaryExpr*)astNode;
        return eval_bin_expr(childObj, env);
    }
    else if(astNode->getKind() == NodeType::PROGRAM){
        Program* childObj = (Program*)astNode;
        return eval_program(childObj, env);
    }
    else if(astNode->getKind() == NodeType::VAR_D){
        VarDeclaration* childObj = (VarDeclaration*)astNode;
        return eval_var_declaration(childObj, env);
    }
    else if(astNode->getKind() == NodeType::FUN_D){
        FunDeclaration* childObj = (FunDeclaration*)astNode;
        return eval_func_declaration(childObj, env);
    }
    else if(astNode->getKind() == NodeType::CONDEXPR){
        CondExpr* childObj = (CondExpr*)astNode;
        return eval_condition(childObj, env);
    }
    else if(astNode->getKind() == NodeType::WHILE_LOOP){
        WhileStmt* childObj = (WhileStmt*)astNode;
        return eval_while(childObj, env);
    }
    else if(astNode->getKind() == NodeType::MEMBEREXPR){
        MemberExpr* childObj = (MemberExpr*)astNode;
        return eval_member_expr(childObj, env);
    }
    else{
        cout << "Eval Error: Unknown type!\n"; // !!! assert ile evezle
        exit(0);
    }
}
