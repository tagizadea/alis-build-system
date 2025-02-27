#include <env.hpp>
#include <operations.hpp>

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
    if(en->variables.find(name) != en->variables.end()) delete en->variables[name];
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

NativeFuncVal* Make_NFunc(FunctionCall call){ // !!! logic issue
    NativeFuncVal* fun = new NativeFuncVal;
    fun->call = call;
    return fun;
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

    // Native Functions
    FunctionCall temp;
    temp.funAddr = n_funs::print;
    env->declareVar("print", Make_NFunc(temp), true);
    temp.funAddr = n_funs::timeNow;
    env->declareVar("timeNow", Make_NFunc(temp), true);
    temp.funAddr = n_funs::floor;
    env->declareVar("floor", Make_NFunc(temp), true);
    temp.funAddr = n_funs::system;
    env->declareVar("system", Make_NFunc(temp), true);
}
