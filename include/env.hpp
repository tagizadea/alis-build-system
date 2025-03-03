#ifndef ENV
#define ENV

#include <lexer.hpp>
#include <parser.hpp>
#include <valtypes.hpp>
#include <unordered_map>
#include <unordered_set>
#include <operations.hpp>

class Env{
    private:
    vector <Env*> children;
    
    public:
    unordered_map <string, Value*> variables;
    unordered_set <string> constants;
    Env* parent = nullptr;
    Value* declareVar(string name, Value* val, bool isConst);
    Value* assignVar(string name, Value* val);
    Env* resolve(string name);
    Value* lookUpVar(string name);
};

NumberVal* Make_Number(long double val);
StringVal* Make_String(std::string val);
BoolValue* Make_Bool(bool b);
NullVal* Make_Null();
NativeFuncVal* Make_NFunc(FunctionCall call);

void InitNatives(Env* env);

#endif