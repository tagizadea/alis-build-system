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
        exit(0); // !!! Debug systemi ile deyis
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

StringVal* Make_String(std::string val){
    StringVal* st = new StringVal;
    st->val = val;
    return st;
}

BoolValue* Make_Bool(bool b){
    BoolValue* val = new BoolValue;
    val->val = b;
    return val;
}

NullVal* Make_Null(){
    return new NullVal;
}

NativeFuncVal* Make_NFunc(FunctionCall call){
    NativeFuncVal* fun = new NativeFuncVal;
    fun->call = call;
    return fun;
}

void InitNatives(Env* env){
    // Strings
    env->declareVar("endl", Make_String("\n"), true);
    #if defined(_WIN32) || defined(_WIN64)
        env->declareVar("HOSTNAME", Make_String("WINDOWS"), true);
    #elif defined(__linux__)
        env->declareVar("HOSTNAME", Make_String("LINUX"), true);
    #elif defined(__APPLE__) && defined(__MACH__)
        env->declareVar("HOSTNAME", Make_String("APPLE"), true);
    #elif defined(__unix__) // all unices not caught above
        env->declareVar("HOSTNAME", Make_String("UNIX"), true);
    #elif defined(__FreeBSD__)
        env->declareVar("HOSTNAME", Make_String("FREEBSD"), true);
    #else
        env->declareVar("HOSTNAME", Make_String("UNKNOWN"), true);
    #endif
    // env->declareVar("CC", Make_String("g++"), false);

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
    temp.funAddr = n_funs::max;
    env->declareVar("max", Make_NFunc(temp), true);
    temp.funAddr = n_funs::min;
    env->declareVar("min", Make_NFunc(temp), true);
    temp.funAddr = n_funs::system;
    env->declareVar("system", Make_NFunc(temp), true);
    temp.funAddr = n_funs::Ntrack;
    env->declareVar("track", Make_NFunc(temp), true);
    temp.funAddr = n_funs::run;
    env->declareVar("run", Make_NFunc(temp), true);
    
    // Native Vector Functions
    ListVecNFuncs.resize(3);
    temp.funAddr = n_funs::vector_size;
    ListVecNFuncs[0] = Make_NFunc(temp);
    temp.funAddr = n_funs::vector_push;
    ListVecNFuncs[1] = Make_NFunc(temp);
    temp.funAddr = n_funs::vector_pop;
    ListVecNFuncs[2] = Make_NFunc(temp);
    temp.funAddr = n_funs::vector_sort;
    ListVecNFuncs[3] = Make_NFunc(temp);
}
