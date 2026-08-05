#include <env.hpp>
using namespace std;
#include <operations.hpp>
#include <debug.hpp>

std::shared_ptr<Value> Env::declareVar(string name, std::shared_ptr<Value> val, bool isConst){
    if(variables.find(name) != variables.end()){
        ABS_FATAL(cat::Env, "env.declare_duplicate", name);
    }

    variables[name] = val;

    if(isConst) constants.insert(name);
    return val;
}

std::shared_ptr<Value> Env::assignVar(string name, std::shared_ptr<Value> val){
    Env* en = this->resolve(name);
    if(en->constants.find(name) != en->constants.end()){
        ABS_FATAL(cat::Env, "env.assign_const", name);
    }
    en->variables[name] = val;
    return val;
}

Env* Env::resolve(string name){
    if(variables.find(name) != variables.end()) return this;

    if(this->parent == nullptr){
        ABS_FATAL(cat::Env, "env.resolve_not_found", name);
    }

    return this->parent->resolve(name);
}

std::shared_ptr<Value> Env::lookUpVar(string name){
    Env* en = this->resolve(name);
    return en->variables[name];
}

std::shared_ptr<NumberVal> Make_Number(long double val){
    auto num = std::make_shared<NumberVal>();
    num->val = val;
    return num;
}

std::shared_ptr<StringVal> Make_String(std::string val){
    auto st = std::make_shared<StringVal>();
    st->val = val;
    return st;
}

std::shared_ptr<BoolValue> Make_Bool(bool b){
    auto val = std::make_shared<BoolValue>();
    val->val = b;
    return val;
}

std::shared_ptr<NullVal> Make_Null(){
    return std::make_shared<NullVal>();
}

std::shared_ptr<NativeFuncVal> Make_NFunc(FunctionCall call){
    auto fun = std::make_shared<NativeFuncVal>();
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

    // Numbers
    env->declareVar("SALAM", Make_Number(10), true);
    env->declareVar("ZERO", std::make_shared<NumberVal>(), true);

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
    temp.funAddr = n_funs::Ndefine;
    env->declareVar("define", Make_NFunc(temp), true);
    temp.funAddr = n_funs::run;
    env->declareVar("run", Make_NFunc(temp), true);
    temp.funAddr = n_funs::Type;
    env->declareVar("type", Make_NFunc(temp), true);
    temp.funAddr = n_funs::ston;
    env->declareVar("StoN", Make_NFunc(temp), true);
    temp.funAddr = n_funs::ntos;
    env->declareVar("NtoS", Make_NFunc(temp), true);
    temp.funAddr = n_funs::set_include;
    env->declareVar("set_include", Make_NFunc(temp), true);
    temp.funAddr = n_funs::compile;
    env->declareVar("compile", Make_NFunc(temp), true);
    temp.funAddr = n_funs::link;
    env->declareVar("link", Make_NFunc(temp), true);
    temp.funAddr = n_funs::set_lang;
    env->declareVar("set_lang", Make_NFunc(temp), true);
    temp.funAddr = n_funs::debug_level;
    env->declareVar("debug_level", Make_NFunc(temp), true);
    temp.funAddr = n_funs::debug_log;
    env->declareVar("debug_log", Make_NFunc(temp), true);
    temp.funAddr = n_funs::glob;
    env->declareVar("glob", Make_NFunc(temp), true);
    
    // Native Vector Functions
    ListVecNFuncs.resize(4); // Increase each time when adding new method
    temp.funAddr = n_funs::vector_size;
    ListVecNFuncs[0] = Make_NFunc(temp);
    temp.funAddr = n_funs::vector_push;
    ListVecNFuncs[1] = Make_NFunc(temp);
    temp.funAddr = n_funs::vector_pop;
    ListVecNFuncs[2] = Make_NFunc(temp);
    temp.funAddr = n_funs::vector_sort;
    ListVecNFuncs[3] = Make_NFunc(temp);
}