#ifndef OPERATIONS
#define OPERATIONS

#include <iostream>
#include <vector>
#include <map>
#include <valtypes.hpp>
#include <functional>
#include <env.hpp>
using namespace std;

vector <string> getSystemFiles(vector <string> &files);
// Printing Abstact Syntax Tree using Statements
void print_stmt(Stmt* stmt, int tab);
// Printing Evaluation
void print_eval(Value* eval, int tab);
// Printing Env
void print_env(Env* env, int tab);


class Env;

namespace n_funs{
    Value* vector_size(vector <Value*> args, Env* env);
    Value* vector_push(vector <Value*> args, Env* env);
    Value* vector_pop(vector <Value*> args, Env* env);
    Value* print(vector <Value*> args, Env* env);
    Value* timeNow(vector <Value*> args, Env* env);
    Value* floor(vector <Value*> args, Env* env);
    Value* max(vector <Value*> args, Env* env);
    Value* min(vector <Value*> args, Env* env);
    Value* system(vector <Value*> args, Env* env);
    Value* compile(vector <Value*> args, Env* env);
}

#endif