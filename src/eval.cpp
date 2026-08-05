#include <eval.hpp>
using namespace std;
#include <debug.hpp>
#include <cmath>

shared_ptr<Value> eval_program(Program* program, Env* env){
    if(program->body.empty()) return env->lookUpVar("Null");
    
    shared_ptr<Value> last;
    
    for(int i=0;i<program->body.size();++i){
        NodeType kind = program->body[i]->getKind();
        if(kind != NodeType::NUMERIC_L && kind != NodeType::STRING_L) 
            last = evaluate(program->body[i].get(), env);
    }

    return last;
}

bool isInteger(long double num) {
    return num == floor(num);
}

shared_ptr<Value> eval_bin_expr(BinaryExpr* binop, Env* env){
    shared_ptr<Value> lhs = evaluate(binop->left.get(), env);
    shared_ptr<Value> rhs = evaluate(binop->right.get(), env);

    if(lhs->getType() == ValueType::String && rhs->getType() == ValueType::String){
        if(binop->op == "+"){
            auto temp = make_shared<StringVal>();
            StringVal* nl = (StringVal*)lhs.get();
            StringVal* nr = (StringVal*)rhs.get();
            temp->val = nl->val + nr->val;
            return temp;
        }
    }

    if(lhs->getType() == ValueType::String && rhs->getType() == ValueType::Number && binop->op == "*"){
        auto temp = make_shared<StringVal>();
        StringVal* nl = (StringVal*)lhs.get();
        NumberVal* nr = (NumberVal*)rhs.get();
        string s = "";
        for(int i=0;i<nr->val;++i) s += nl->val;
        temp->val = s;
        return temp;
    }
    else if(lhs->getType() == ValueType::Number && rhs->getType() == ValueType::String && binop->op == "*"){
        auto temp = make_shared<StringVal>();
        NumberVal* nl = (NumberVal*)lhs.get();
        StringVal* nr = (StringVal*)rhs.get();
        string s = "";
        for(int i=0;i<nl->val;++i) s += nr->val;
        temp->val = s;
        return temp;
    }

    if(lhs->getType() == ValueType::Bool && rhs->getType() == ValueType::Bool){
        if(binop->op == "&&"){
            auto temp = make_shared<BoolValue>();
            BoolValue* nl = (BoolValue*)lhs.get();
            BoolValue* nr = (BoolValue*)rhs.get();
            temp->val = nl->val && nr->val;
            return temp;
        }
        if(binop->op == "||"){
            auto temp = make_shared<BoolValue>();
            BoolValue* nl = (BoolValue*)lhs.get();
            BoolValue* nr = (BoolValue*)rhs.get();
            temp->val = nl->val || nr->val;
            return temp;
        }
        if(binop->op == "=="){
            auto temp = make_shared<BoolValue>();
            BoolValue* nl = (BoolValue*)lhs.get();
            BoolValue* nr = (BoolValue*)rhs.get();
            temp->val = nl->val == nr->val;
            return temp;
        }
        if(binop->op == "!="){
            auto temp = make_shared<BoolValue>();
            BoolValue* nl = (BoolValue*)lhs.get();
            BoolValue* nr = (BoolValue*)rhs.get();
            temp->val = nl->val != nr->val;
            return temp;
        }
    }

    if(lhs->getType() == ValueType::Number && rhs->getType() == ValueType::Number){
        if(binop->op == "+"){
            auto temp = make_shared<NumberVal>();
            NumberVal* nl = (NumberVal*)lhs.get();
            NumberVal* nr = (NumberVal*)rhs.get();
            temp->val = nl->val + nr->val;
            return temp;
        }
        if(binop->op == "-"){
            auto temp = make_shared<NumberVal>();
            NumberVal* nl = (NumberVal*)lhs.get();
            NumberVal* nr = (NumberVal*)rhs.get();
            temp->val = nl->val - nr->val;
            return temp;
        }
        if(binop->op == "*"){
            auto temp = make_shared<NumberVal>();
            NumberVal* nl = (NumberVal*)lhs.get();
            NumberVal* nr = (NumberVal*)rhs.get();
            temp->val = nl->val * nr->val;
            return temp;
        }
        if(binop->op == "/"){
            auto temp = make_shared<NumberVal>();
            NumberVal* nl = (NumberVal*)lhs.get();
            NumberVal* nr = (NumberVal*)rhs.get();
            temp->val = nl->val / nr->val;
            return temp;
        }
        if(binop->op == "%"){
            auto temp = make_shared<NumberVal>();
            NumberVal* nl = (NumberVal*)lhs.get();
            NumberVal* nr = (NumberVal*)rhs.get();
            if(!isInteger(nl->val) || !isInteger(nr->val)){
                ABS_FATAL(cat::Eval, "eval.mod_not_integer");
            }
            long long left = nl->val;
            long long right = nr->val;
            temp->val = left % right;
            return temp;
        }
        if(binop->op == ">"){
            auto temp = make_shared<BoolValue>();
            NumberVal* nl = (NumberVal*)lhs.get();
            NumberVal* nr = (NumberVal*)rhs.get();
            temp->val = nl->val > nr->val;
            return temp;
        }
        if(binop->op == "<"){
            auto temp = make_shared<BoolValue>();
            NumberVal* nl = (NumberVal*)lhs.get();
            NumberVal* nr = (NumberVal*)rhs.get();
            temp->val = nl->val < nr->val;
            return temp;
        }
        if(binop->op == ">="){
            auto temp = make_shared<BoolValue>();
            NumberVal* nl = (NumberVal*)lhs.get();
            NumberVal* nr = (NumberVal*)rhs.get();
            temp->val = nl->val >= nr->val;
            return temp;
        }
        if(binop->op == "<="){
            auto temp = make_shared<BoolValue>();
            NumberVal* nl = (NumberVal*)lhs.get();
            NumberVal* nr = (NumberVal*)rhs.get();
            temp->val = nl->val <= nr->val;
            return temp;
        }
        if(binop->op == "=="){
            auto temp = make_shared<BoolValue>();
            NumberVal* nl = (NumberVal*)lhs.get();
            NumberVal* nr = (NumberVal*)rhs.get();
            temp->val = nl->val == nr->val;
            return temp;
        }
        if(binop->op == "!="){
            auto temp = make_shared<BoolValue>();
            NumberVal* nl = (NumberVal*)lhs.get();
            NumberVal* nr = (NumberVal*)rhs.get();
            temp->val = nl->val != nr->val;
            return temp;
        }
    }

    return env->lookUpVar("Null");
}

shared_ptr<Value> eval_object_expr(ObjectLiteral* obj, Env* env){
    auto object = make_shared<ObjectValue>();

    for(const auto& i : obj->properties){

        shared_ptr<Value> val = (i->val == nullptr) ? (env->lookUpVar(i->key)) : (evaluate(i->val.get(), env));

        object->properties[i->key] = val;
    }
    return object;
}

shared_ptr<Value> eval_call_expr(CallExpr* expr, Env* env){
    vector <shared_ptr<Value>> args;
    
    for(const auto& i : expr->args){
        args.push_back(evaluate(i.get(), env));
    }

    shared_ptr<Value> fn = evaluate(expr->callexpr.get(), env);

    if(fn->getType() == ValueType::NFUNC){
        NativeFuncVal* nfn = (NativeFuncVal*)fn.get();
        if(nfn->list){
            for(const auto& i : nfn->call.args) args.push_back(i);
        }
        return nfn->call.funAddr(args, env);
    }
    else if(fn->getType() == ValueType::FUNC){
        FunctionVal* func = (FunctionVal*)fn.get();
        if(args.size() != func->params.size()){
            ABS_FATAL(cat::Eval, "eval.missing_args", func->name);
        }
        auto scope = make_unique<Env>();
        scope->parent = func->decEnv;
        for(int i = 0; i < func->params.size(); ++i){
            scope->declareVar(func->params[i], args[i], false);
        }
        if(func->body.empty()) return env->lookUpVar("Null");
        shared_ptr<Value> ret;
        for(const auto& i : func->body){
            ret = evaluate(i.get(), scope.get());
        }
        return ret;
    }
    ABS_FATAL(cat::Eval, "eval.call_non_function");
    return nullptr;
}

shared_ptr<Value> eval_ident(Identifier* idn, Env* env){
    return env->lookUpVar(idn->symbol);
}

shared_ptr<Value> eval_var_declaration(VarDeclaration* var_d, Env* env){
    shared_ptr<Value> temp;
    for(auto& i : var_d->vars){
        if(i.second->getKind() == NodeType::BREAK || i.second->getKind() == NodeType::CONTINUE){
            ABS_FATAL(cat::Eval, "eval.assign_break_continue");
        }
        shared_ptr<Value> value = (i.second != nullptr) ? (evaluate(i.second.get(), env)) : (env->lookUpVar("Null"));
        temp = env->declareVar(i.first, value, var_d->constant);
    }

    return temp;
}

shared_ptr<Value> eval_var_assignment(AssignExpr* as, Env* env){
    if(as->assignexpr->getKind() == NodeType::MEMBEREXPR){
        MemberExpr* temp = (MemberExpr*)as->assignexpr.get();
        shared_ptr<Value> obj_v = evaluate(temp->object.get(), env);
        switch(obj_v->getType()){
            case ValueType::Object :
            if(temp->property->getKind() == NodeType::IDENTIFIER && temp->object->getKind() == NodeType::IDENTIFIER){
                Identifier* property_key = (Identifier*)temp->property.get();
                Identifier* obj_key = (Identifier*)temp->object.get();
                auto obj = static_pointer_cast<ObjectValue>(obj_v);
                obj = obj->clone();
                env->assignVar(obj_key->symbol, obj);
                obj->properties[property_key->symbol] = evaluate(as->value.get(), env);
            }
            break;
        case ValueType::List :
            if(temp->property->getKind() == NodeType::NUMERIC_L && temp->object->getKind() == NodeType::IDENTIFIER){
                NumberVal* index = (NumberVal*)evaluate(temp->property.get(), env).get();
                Identifier* key = (Identifier*)temp->object.get();
                auto l = static_pointer_cast<ListValue>(obj_v);
                if(index->val >= 0 && index->val < l->v.size()){
                    l = l->clone();
                    env->assignVar(key->symbol, l);
                    l->v[index->val] = evaluate(as->value.get(), env);
                }
            }
            break;
        default:
            ABS_FATAL(cat::Eval, "eval.assign_member_type");
        }
        return env->lookUpVar("Null");
    }

    if(as->assignexpr->getKind() != NodeType::IDENTIFIER){
        ABS_FATAL(cat::Eval, "eval.assign_left_not_identifier");
    }
    if(as->value->getKind() == NodeType::BREAK || as->value->getKind() == NodeType::CONTINUE){
        ABS_FATAL(cat::Eval, "eval.assign_right_break_continue");
    }
    string varname = ((Identifier*)(as->assignexpr.get()))->symbol;
    return env->assignVar(varname, evaluate(as->value.get(), env));
}

shared_ptr<Value> eval_condition(CondExpr* cond, Env* env){
    shared_ptr<Value> condition = evaluate(cond->condition.get(), env);
    
    if(condition->getType() != ValueType::Bool){
        ABS_FATAL(cat::Eval, "eval.if_not_bool");
    }

    BoolValue* temp = (BoolValue*)condition.get();
    auto scope = make_unique<Env>();
    scope->parent = env;
    if(temp->val){
        for(const auto& i : cond->ThenBranch){
            shared_ptr<Value> res;
            res = evaluate(i.get(), scope.get());
            if(res->getType() == ValueType::Break || res->getType() == ValueType::Continue) return res;
        }
    }
    else{
        for(const auto& i : cond->ElseBranch){
            shared_ptr<Value> res;
            res = evaluate(i.get(), scope.get());
            if(res->getType() == ValueType::Break || res->getType() == ValueType::Continue) return res;
        }
    }
    return env->lookUpVar("Null");
}

shared_ptr<Value> eval_while(WhileStmt* wh, Env* env){
    shared_ptr<Value> condition = evaluate(wh->condition.get(), env);
    ++env->loop_depth;
    if(condition->getType() != ValueType::Bool){
        ABS_FATAL(cat::Eval, "eval.while_not_bool");
    }

    BoolValue* temp = (BoolValue*)condition.get();

    while(temp->val){
        auto scope = make_unique<Env>();
        scope->parent = env;
        scope->loop_depth = env->loop_depth;
        bool br = false;
        for(const auto& i : wh->ThenBranch){
            shared_ptr<Value> res;
            res = evaluate(i.get(), scope.get());
            if(res->getType() == ValueType::Break){
                br = true;
                break;
            }
            if(res->getType() == ValueType::Continue){
                break;
            }
        }
        if(br) break;
        condition = evaluate(wh->condition.get(), env);
        temp = (BoolValue*)condition.get();
    }
    --env->loop_depth;
    return env->lookUpVar("Null");
}

shared_ptr<Value> eval_for(ForStmt* fr, Env* env){
    ++env->loop_depth;
    auto scope = make_unique<Env>();
    scope->parent = env;
    scope->loop_depth = env->loop_depth;

    evaluate(fr->iterator_dec.get(), scope.get());

    shared_ptr<Value> condition = evaluate(fr->condition.get(), scope.get());

    if(condition->getType() != ValueType::Bool){
        ABS_FATAL(cat::Eval, "eval.for_not_bool");
    }

    BoolValue* temp = (BoolValue*)condition.get();

    while(temp->val){
        auto scope_d = make_unique<Env>();
        scope_d->parent = scope.get();
        scope_d->loop_depth = scope->loop_depth;

        bool br = false;
        for(const auto& i : fr->ThenBranch){
            shared_ptr<Value> res;
            res = evaluate(i.get(), scope_d.get());
            if(res->getType() == ValueType::Break){
                br = true;
                break;
            }
            if(res->getType() == ValueType::Continue){
                break;
            }
        }
        if(br) break;
        evaluate(fr->operation.get(), scope.get());
        condition = evaluate(fr->condition.get(), scope.get());
        temp = (BoolValue*)condition.get();
    }
    --env->loop_depth;
    return env->lookUpVar("Null");
}

shared_ptr<Value> eval_member_val_expr(MemberExpr* me, Env* env){
    if(me->property == nullptr) return env->lookUpVar("Null");

    shared_ptr<Value> obj_v = evaluate(me->object.get(), env);

    if(obj_v->getType() == ValueType::List){
        auto list = static_pointer_cast<ListValue>(obj_v);
        if(me->computed){
            shared_ptr<Value> i = evaluate(me->property.get(), env);
            if(i->getType() != ValueType::Number){
                ABS_FATAL(cat::Eval, "eval.index_not_numeric", (int)i->getType());
            }
            try{
                return list->v.at( int(((NumberVal*)i.get())->val) );
            }
            catch(const exception& e){
                ABS_FATAL(cat::Eval, "eval.index_out_of_bounds", e.what());
            }
        }
        else{
            string name = ((Identifier*)me->property.get())->symbol;
            shared_ptr<NativeFuncVal> tnf = ListVecNFuncs[0];
            if(name == "size") tnf = ListVecNFuncs[0];
            else if(name == "push") tnf = ListVecNFuncs[1];
            else if(name == "pop") tnf = ListVecNFuncs[2];
            else if(name == "sort") tnf = ListVecNFuncs[3];
            else{
                ABS_FATAL(cat::Eval, "eval.unknown_list_func");
            }
            tnf->list = true;
            tnf->call.args.clear();
            tnf->call.args.push_back(list);
            tnf->call.env = env;
            return tnf;
        }
    }

    if(obj_v->getType() == ValueType::Object){
        if(me->property->getKind() == NodeType::IDENTIFIER){
            Identifier* key = (Identifier*)me->property.get();
            auto obj = static_pointer_cast<ObjectValue>(obj_v);
            return obj->properties[key->symbol];
        }
        else if(me->property->getKind() == NodeType::CALLEXPR){
            ABS_FATAL(cat::Eval, "eval.call_in_object");
        }
    }
    
    return evaluate(me->property.get(), env);
}

shared_ptr<Value> eval_func_declaration(FunDeclaration* fn, Env* env){
    auto fn_val = make_shared<FunctionVal>();
    fn_val->name = fn->name;
    fn_val->params = fn->parameters;
    fn_val->decEnv = env;
    // Transfer ownership of the function body AST nodes to the FunctionVal.
    // This lets the function outlive the Program that was parsed (e.g. via run()).
    fn_val->body = move(fn->body);
    return env->declareVar(fn->name, fn_val, true);
}

shared_ptr<Value> eval_list_expr(ListLiteral* l, Env* env){
    auto list_val = make_shared<ListValue>();
    vector <shared_ptr<Value>> v(l->properties.size());
    for(int i=0;i<l->properties.size();++i){
        ElementLiteral* el = l->properties[i].get();
        v[el->key] = evaluate(el->val.get(), env);

        int val_id = (int)v[i]->getType();
        if(list_val->mapTypeCounter[val_id] == 0) ++list_val->distinc_types;
        ++list_val->mapTypeCounter[val_id];
        if(list_val->distinc_types == 1) list_val->consist_of = v[i]->getType();
        else list_val->consist_of = ValueType::None;
    }
    list_val->v = v;
    return list_val;
}

shared_ptr<Value> eval_unary_val_expr(UnaryExpr* l, Env* env){
    auto temp = static_pointer_cast<NumberVal>(evaluate(l->identifier.get(), env));
    if(l->left){
        if(l->plus) ++temp->val;
        else --temp->val;
    }
    else{
        auto result = make_shared<NumberVal>();
        result->val = temp->val;

        if(l->plus) ++temp->val;
        else --temp->val;

        return result;
    }

    return temp;
}

shared_ptr<Value> evaluate(Stmt* astNode, Env* env){
    ABS_PROFILE_SCOPE("evaluate");
    if(astNode->getKind() == NodeType::NUMERIC_L){ //
        NumericLiteral* childObj = (NumericLiteral*)astNode;
        auto temp = make_shared<NumberVal>();
        temp->val = childObj->val;
        return temp;
    }
    else if(astNode->getKind() == NodeType::NOTEXPR){ //
        NotExpr* childObj = (NotExpr*)astNode;
        shared_ptr<Value> val = evaluate(childObj->val.get(), env);
        if(val->getType() != ValueType::Bool){
            ABS_FATAL(cat::Eval, "eval.not_not_bool");
        }
        auto temp = make_shared<BoolValue>();
        temp->val = !((BoolValue*)val.get())->val;
        return temp;
    }
    else if(astNode->getKind() == NodeType::STRING_L){
        StringLiteral* childObj = (StringLiteral*)astNode;
        auto temp = make_shared<StringVal>();
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
        if(env->loop_depth <= 0){
            ABS_FATAL(cat::Eval, "eval.continue_outside_loop");
        }
        return make_shared<ContinueVal>();
    }
    else if(astNode->getKind() == NodeType::BREAK){ // SAFE
        if(env->loop_depth <= 0){
            ABS_FATAL(cat::Eval, "eval.break_outside_loop");
        }
        return make_shared<BreakVal>();
    }
    else{
        ABS_FATAL(cat::Eval, "eval.unknown_type");
    }
}