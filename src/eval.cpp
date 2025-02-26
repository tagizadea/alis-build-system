#include <eval.hpp>
#include <cmath>

shared_ptr<Value> eval_program(shared_ptr<Program> program, shared_ptr<Env> env){
    shared_ptr<Value> last = make_shared<NullVal>();

    for(int i=0;i<program->body.size();++i)
        last = evaluate(program->body[i], env);

    return last;
}

bool isInteger(long double num) {
    return num == std::floor(num);
}

shared_ptr<Value> eval_bin_expr(shared_ptr<BinaryExpr> binop, shared_ptr<Env> env){
    shared_ptr<Value> lhs = evaluate(binop->left, env);
    shared_ptr<Value> rhs = evaluate(binop->right, env);

    if(lhs->getType() == ValueType::Number && rhs->getType() == ValueType::Number){
        if(binop->op == "+"){
            shared_ptr<NumberVal> temp = make_shared<NumberVal>();
            shared_ptr<NumberVal> nl = static_pointer_cast<NumberVal>(lhs);
            shared_ptr<NumberVal> nr = static_pointer_cast<NumberVal>(rhs);
            temp->val = nl->val + nr->val;
            return temp;
        }
        if(binop->op == "-"){
            shared_ptr<NumberVal> temp = make_shared<NumberVal>();
            shared_ptr<NumberVal> nl = static_pointer_cast<NumberVal>(lhs);
            shared_ptr<NumberVal> nr = static_pointer_cast<NumberVal>(rhs);
            temp->val = nl->val - nr->val;
            return temp;
        }
        if(binop->op == "*"){
            shared_ptr<NumberVal> temp = make_shared<NumberVal>();
            shared_ptr<NumberVal> nl = static_pointer_cast<NumberVal>(lhs);
            shared_ptr<NumberVal> nr = static_pointer_cast<NumberVal>(rhs);
            temp->val = nl->val * nr->val;
            return temp;
        }
        if(binop->op == "/"){
            shared_ptr<NumberVal> temp = make_shared<NumberVal>();
            shared_ptr<NumberVal> nl = static_pointer_cast<NumberVal>(lhs);
            shared_ptr<NumberVal> nr = static_pointer_cast<NumberVal>(rhs);
            temp->val = nl->val / nr->val;
            return temp;
        }
        if(binop->op == "%"){
            shared_ptr<NumberVal> temp = make_shared<NumberVal>();
            shared_ptr<NumberVal> nl = static_pointer_cast<NumberVal>(lhs);
            shared_ptr<NumberVal> nr = static_pointer_cast<NumberVal>(rhs);
            if(!isInteger(nl->val) || !isInteger(nr->val)){
                cout << "Evaluation Error: Kesr ededlerin MOD-u tapilmir!";
                exit(0); // !!! Debug systemi ile deyis
            }
            long long left = nl->val;
            long long right = nr->val;
            temp->val = left % right;
            return temp;
        }
    }

    return Make_Null();
}

shared_ptr<Value> eval_object_expr(shared_ptr<ObjectLiteral> obj, shared_ptr<Env> env){
    shared_ptr<ObjectValue> object = make_shared<ObjectValue>();

    for(const shared_ptr<PropertyLiteral> i : obj->properties){
        shared_ptr<Value> val = (i->val == nullptr) ? (env->lookUpVar(i->key)) : (evaluate(i->val, env));
        object->properties[i->key] = val;
    }

    return object;
}

shared_ptr<Value> eval_ident(shared_ptr<Identifier> idn, shared_ptr<Env> env){
    shared_ptr<Value> val = env->lookUpVar(idn->symbol);
    return val;
}

shared_ptr<Value> eval_var_declaration(shared_ptr<VarDeclaration> var_d, shared_ptr<Env> env){
    shared_ptr<Value> value = var_d->val ? (evaluate(var_d->val, env)) : (Make_Null());
    return env->declareVar(var_d->identifier, value, var_d->constant);
}

shared_ptr<Value> eval_var_assignment(shared_ptr<AssignExpr> as, shared_ptr<Env> env){
    if(as->assignexpr->getKind() != NodeType::IDENTIFIER){
        cout << "Evaluation Error: Assignmentdə sol identifier işlənməyib!";
        exit(0); // !!! Debug ile deyis
    }
    //string varname = ((Identifier*)(as->assignexpr))->symbol;
    string varname = (static_pointer_cast<Identifier>(as->assignexpr))->symbol;
    return env->assignVar(varname, evaluate(as->value, env));
}

shared_ptr <Value> evaluate(shared_ptr <Stmt> astNode, shared_ptr <Env> env){
    if(astNode->getKind() == NodeType::NUMERIC_L){
        shared_ptr<NumericLiteral> childObj = static_pointer_cast<NumericLiteral>(astNode);
        //NumberVal* temp = new NumberVal;
        shared_ptr <NumberVal> temp = make_shared<NumberVal>();
        temp->val = childObj->val;
        return temp;
    }
    else if(astNode->getKind() == NodeType::IDENTIFIER){
        shared_ptr<Identifier> childObj = static_pointer_cast<Identifier>(astNode);
        return eval_ident(childObj, env);
    }
    else if(astNode->getKind() == NodeType::OBJECT_L){
        shared_ptr<ObjectLiteral> childObj = static_pointer_cast<ObjectLiteral>(astNode);
        return eval_object_expr(childObj, env);
    }
    else if(astNode->getKind() == NodeType::ASSIGNEXPR){
        shared_ptr<AssignExpr> childObj = static_pointer_cast<AssignExpr>(astNode);
        return eval_var_assignment(childObj, env);
    }
    else if(astNode->getKind() == NodeType::BINARYEXPR){
        shared_ptr<BinaryExpr> childObj = static_pointer_cast<BinaryExpr>(astNode);
        return eval_bin_expr(childObj, env);
    }
    else if(astNode->getKind() == NodeType::PROGRAM){
        shared_ptr<Program> childObj = static_pointer_cast<Program>(astNode);
        return eval_program(childObj, env);
    }
    else if(astNode->getKind() == NodeType::VAR_D){
        shared_ptr<VarDeclaration> childObj = static_pointer_cast<VarDeclaration>(astNode);
        return eval_var_declaration(childObj, env);
    }
    else{
        cout << "Eval Error: Unknown type!\n"; // !!! assert ile evezle
        exit(0);
    }
}
