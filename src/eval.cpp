#include <eval.hpp>

Value* eval_program(Program* program){
    Value* last = new NullVal;

    for(int i=0;i<program->body.size();++i)
        last = evaluate(program->body[i]);

    return last;
}

Value* eval_bin_expr(BinaryExpr* binop){
    Value* lhs = evaluate(binop->left);
    Value* rhs = evaluate(binop->right);

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

Value* evaluate(Stmt* astNode){
    if(astNode->getKind() == NodeType::NUMERIC_L){
        NumericLiteral* childObj = (NumericLiteral*)astNode;
        NumberVal* temp = new NumberVal;
        temp->val = childObj->val;
        return temp;
    }
    else if(astNode->getKind() == NodeType::NULL_L){
        return new NullVal;
    }
    else if(astNode->getKind() == NodeType::BINARYEXPR){
        BinaryExpr* childObj = (BinaryExpr*)astNode;
        return eval_bin_expr(childObj);
    }
    else if(astNode->getKind() == NodeType::PROGRAM){
        Program* childObj = (Program*)astNode;
        return eval_program(childObj);
    }
    else{
        cout << "Eval Error: Unknown type!\n"; // !!! assert ile evezle
        exit(0);
    }
}
