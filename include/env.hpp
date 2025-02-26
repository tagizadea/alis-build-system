#ifndef ENV
#define ENV

#include <lexer.hpp>
#include <parser.hpp>
#include <valtypes.hpp>
#include <unordered_map>

class Env{
    private:
    Env* parent = nullptr;
    vector <Env*> children;
    unordered_map <string, Value*> variables;
    
    public:
    Value* declareVar(string name, Value* val);
    Value* assignVar(string name, Value* val);
    Env* resolve(string name);
    Value* lookUpVar(string name);
};

NumberVal* Make_Number(long double val);
BoolValue* Make_Bool(bool b);
NullVal* Make_Null();

void InitNatives(Env* env);

#endif