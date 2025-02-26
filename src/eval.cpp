#include <eval.hpp>

Value* eval_program(Program* program, Env* env){
    Value* last = new NullVal;

    for(int i=0;i<program->body.size();++i)
        last = evaluate(program->body[i], env);

    return last;
}

Value* eval_bin_expr(BinaryExpr* binop, Env* env){
    Value* lhs = evaluate(binop->left, env);
    Value* rhs = evaluate(binop->right, env);

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
    }

    return new NullVal;
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

Value* evaluate(Stmt* astNode, Env* env){
    if(astNode->getKind() == NodeType::NUMERIC_L){
        NumericLiteral* childObj = (NumericLiteral*)astNode;
        NumberVal* temp = new NumberVal;
        temp->val = childObj->val;
        return temp;
    }
    else if(astNode->getKind() == NodeType::IDENTIFIER){
        Identifier* childObj = (Identifier*)astNode;
        return eval_ident(childObj, env);
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
