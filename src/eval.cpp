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

    if(fn->getType() != ValueType::NFUNC){
        cout << "Evaluation Error: Cannot call value that is not a function";
        exit(0); // !!! Debug systemi ile deyis
    }
    

    Value* result = ((NativeFuncVal*)fn)->call.funAddr(args, env);
    return result;
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
    else{
        cout << "Eval Error: Unknown type!\n"; // !!! assert ile evezle
        exit(0);
    }
}
