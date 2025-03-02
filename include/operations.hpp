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

class Env;

namespace n_funs{
    Value* print(vector <Value*> args, Env* env);
    Value* timeNow(vector <Value*> args, Env* env);
    Value* floor(vector <Value*> args, Env* env);
    Value* max(vector <Value*> args, Env* env);
    Value* min(vector <Value*> args, Env* env);
    Value* system(vector <Value*> args, Env* env);
}

#endif