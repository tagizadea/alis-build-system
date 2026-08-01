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
using namespace std;

vector <string> getSystemFiles(vector <string> &files);
// Printing Abstact Syntax Tree using Statements
void print_stmt(Stmt* stmt, int tab);
// Printing Evaluation
void print_eval(Value* eval, int tab);
// Printing Env
void print_env(Env* env, int tab);

// Executes a shell command using POSIX fork/exec/waitpid.
// Returns the exit status of the command, or -1 if the command could not be executed.
int exec(const string& cmd);


class Env;

extern vector <NativeFuncVal*> ListVecNFuncs;

namespace sort_comps{
    bool cmp_less_Number(Value* a, Value* b);
    bool cmp_less_String(Value* a, Value* b);
}

namespace n_funs{
    Value* vector_size(vector <Value*> args, Env* env);
    Value* vector_push(vector <Value*> args, Env* env);
    Value* vector_pop(vector <Value*> args, Env* env);
    Value* vector_sort(vector <Value*> args, Env* env);
    Value* print(vector <Value*> args, Env* env);
    Value* timeNow(vector <Value*> args, Env* env);
    Value* floor(vector <Value*> args, Env* env);
    Value* max(vector <Value*> args, Env* env);
    Value* min(vector <Value*> args, Env* env);
    Value* system(vector <Value*> args, Env* env);
    Value* Ntrack(vector <Value*> args, Env* env);
    Value* Ndefine(vector <Value*> args, Env* env);
    Value* Type(vector <Value*> args, Env* env);
    Value* ston(vector <Value*> args, Env* env);
    Value* ntos(vector <Value*> args, Env* env);
    Value* compile(vector <Value*> args, Env* env);
    Value* link(vector <Value*> args, Env* env);
    Value* run(vector <Value*> args, Env* env);
    Value* scan(vector <Value*> args, Env* env);
    Value* set_include(vector <Value*> args, Env* env);

}

#endif