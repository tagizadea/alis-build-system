#ifndef OPERATIONS
#define OPERATIONS

#include <iostream>
#include <vector>
#include <map>
#include <valtypes.hpp>
#include <functional>
#include <env.hpp>
#include <algorithm>
#include <manager.hpp>
#include <eval.hpp>
#include <memory>

std::vector <std::string> getSystemFiles(std::vector <std::string> &files);
// Printing Abstact Syntax Tree using Statements
void print_stmt(Stmt* stmt, int tab);
// Printing Evaluation
void print_eval(std::shared_ptr<Value> eval, int tab);
// Printing Env
void print_env(Env* env, int tab);

// Executes a shell command using POSIX fork/exec/waitpid.
// Returns the exit status of the command, or -1 if the command could not be executed.
int exec(const std::string& cmd);


class Env;

extern std::vector <std::shared_ptr<NativeFuncVal>> ListVecNFuncs;

namespace sort_comps{
    bool cmp_less_Number(std::shared_ptr<Value> a, std::shared_ptr<Value> b);
    bool cmp_less_String(std::shared_ptr<Value> a, std::shared_ptr<Value> b);
}

namespace n_funs{
    std::shared_ptr<Value> vector_size(std::vector <std::shared_ptr<Value>> args, Env* env);
    std::shared_ptr<Value> vector_push(std::vector <std::shared_ptr<Value>> args, Env* env);
    std::shared_ptr<Value> vector_pop(std::vector <std::shared_ptr<Value>> args, Env* env);
    std::shared_ptr<Value> vector_sort(std::vector <std::shared_ptr<Value>> args, Env* env);
    std::shared_ptr<Value> print(std::vector <std::shared_ptr<Value>> args, Env* env);
    std::shared_ptr<Value> timeNow(std::vector <std::shared_ptr<Value>> args, Env* env);
    std::shared_ptr<Value> floor(std::vector <std::shared_ptr<Value>> args, Env* env);
    std::shared_ptr<Value> max(std::vector <std::shared_ptr<Value>> args, Env* env);
    std::shared_ptr<Value> min(std::vector <std::shared_ptr<Value>> args, Env* env);
    std::shared_ptr<Value> system(std::vector <std::shared_ptr<Value>> args, Env* env);
    std::shared_ptr<Value> Ntrack(std::vector <std::shared_ptr<Value>> args, Env* env);
    std::shared_ptr<Value> Ndefine(std::vector <std::shared_ptr<Value>> args, Env* env);
    std::shared_ptr<Value> Type(std::vector <std::shared_ptr<Value>> args, Env* env);
    std::shared_ptr<Value> ston(std::vector <std::shared_ptr<Value>> args, Env* env);
    std::shared_ptr<Value> ntos(std::vector <std::shared_ptr<Value>> args, Env* env);
    std::shared_ptr<Value> compile(std::vector <std::shared_ptr<Value>> args, Env* env);
    std::shared_ptr<Value> link(std::vector <std::shared_ptr<Value>> args, Env* env);
    std::shared_ptr<Value> run(std::vector <std::shared_ptr<Value>> args, Env* env);
    std::shared_ptr<Value> scan(std::vector <std::shared_ptr<Value>> args, Env* env);
    std::shared_ptr<Value> set_include(std::vector <std::shared_ptr<Value>> args, Env* env);
    std::shared_ptr<Value> set_lang(std::vector <std::shared_ptr<Value>> args, Env* env);
    std::shared_ptr<Value> debug_level(std::vector <std::shared_ptr<Value>> args, Env* env);
    std::shared_ptr<Value> debug_log(std::vector <std::shared_ptr<Value>> args, Env* env);
    std::shared_ptr<Value> glob(std::vector <std::shared_ptr<Value>> args, Env* env);
    std::shared_ptr<Value> load_plugin(std::vector <std::shared_ptr<Value>> args, Env* env);

}

#endif