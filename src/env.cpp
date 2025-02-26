#include <env.hpp>

Value* Env::declareVar(string name, Value *val, bool isConst){
    if(variables.find(name) != variables.end()){
        cout << "Declare Error: There is already variable " << name;
        exit(0);
    }

    variables[name] = val;

    if(isConst) constants.insert(name);
    return val;
}

Value* Env::assignVar(string name, Value *val){
    Env* en = this->resolve(name);
    if(en->constants.find(name) != en->constants.end()){
        cout << "Assign Error: Constant deyer assign olanmaz! - " << name;
        exit(0); // !!! Debug systemi ile deyis
    }
    en->variables[name] = val;
    return val;
}

Env* Env::resolve(string name){
    if(variables.find(name) != variables.end()) return this;

    if(this->parent == nullptr){
        cout << "Variable Resolve Error: " << name << " does not exist";
        exit(0);
    }

    return this->parent->resolve(name);
}

Value* Env::lookUpVar(string name){
    Env* en = this->resolve(name);
    return en->variables[name];
}

NumberVal* Make_Number(long double val){
    NumberVal* num = new NumberVal;
    num->val = val;
    return num;
}

BoolValue* Make_Bool(bool b){
    BoolValue* val = new BoolValue;
    val->val = b;
    return val;
}

NullVal* Make_Null(){
    return new NullVal;
}

void InitNatives(Env* env){
    // Numbers
    env->declareVar("SALAM", Make_Number(10), true);
    env->declareVar("ZERO", new NumberVal, true);

    // Bools
    env->declareVar("true", Make_Bool(true), true);
    env->declareVar("false", Make_Bool(false), true);

    // Null
    env->declareVar("Null", Make_Null(), true);
}
