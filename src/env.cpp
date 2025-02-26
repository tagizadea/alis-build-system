#include <env.hpp>

Value* Env::declareVar(string name, Value *val){
    if(variables.find(name) != variables.end()){
        cout << "Declare Error: There is already variable " << name;
        exit(0);
    }

    variables[name] = val;
    return val;
}

Value* Env::assignVar(string name, Value *val){
    Env* en = this->resolve(name);
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
    env->declareVar("SALAM", Make_Number(10));
    env->declareVar("ZERO", new NumberVal);

    // Bools
    env->declareVar("true", Make_Bool(true));
    env->declareVar("false", Make_Bool(false));

    // Null
    env->declareVar("Null", Make_Null());
}
