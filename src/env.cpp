#include <env.hpp>

shared_ptr <Value> Env::declareVar(string name, shared_ptr <Value> val, bool isConst){
    if(variables.find(name) != variables.end()){
        cout << "Declare Error: There is already variable " << name;
        exit(0);
    }

    variables[name] = val;

    if(isConst) constants.insert(name);
    return val;
}

shared_ptr <Value> Env::assignVar(string name, shared_ptr <Value> val){
    shared_ptr <Env> en = this->resolve(name);
    if(en->constants.find(name) != en->constants.end()){
        cout << "Assign Error: Constant deyer assign olanmaz! - " << name;
        exit(0); // !!! Debug systemi ile deyis
    }
    en->variables[name] = val;
    return val;
}

shared_ptr <Env> Env::resolve(string name){
    if(variables.find(name) != variables.end()) return shared_from_this(); //

    if(shared_from_this()->parent == nullptr){
        cout << "Variable Resolve Error: " << name << " does not exist";
        exit(0);
    }

    return shared_from_this()->parent->resolve(name);
}

shared_ptr <Value> Env::lookUpVar(string name){
    shared_ptr <Env> en = resolve(name);
    return en->variables[name];
}

shared_ptr <NumberVal> Make_Number(long double val){
    shared_ptr <NumberVal> num = make_shared<NumberVal>();
    num->val = val;
    return num;
}

shared_ptr <BoolValue> Make_Bool(bool b){
    shared_ptr <BoolValue> val = make_shared<BoolValue>();
    val->val = b;
    return val;
}

shared_ptr <NullVal> Make_Null(){
    return make_shared<NullVal>();
}

void InitNatives(Env* env){
    // Numbers
    env->declareVar("SALAM", Make_Number(10), true);
    env->declareVar("ZERO", make_shared <NumberVal> (), true);

    // Bools
    env->declareVar("true", Make_Bool(true), true);
    env->declareVar("false", Make_Bool(false), true);

    // Null
    env->declareVar("Null", Make_Null(), true);
}
