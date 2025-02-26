#ifndef ENV
#define ENV

#include <lexer.hpp>
#include <parser.hpp>
#include <valtypes.hpp>
#include <unordered_map>
#include <unordered_set>
#include <memory>

class Env : public std::enable_shared_from_this <Env>{
    private:
    shared_ptr <Env> parent = nullptr;
    vector <shared_ptr <Env> > children;
    unordered_map <string, shared_ptr<Value>> variables;
    unordered_set <string> constants;

    public:
    shared_ptr <Value> declareVar(string name, shared_ptr <Value> val, bool isConst);
    shared_ptr <Value> assignVar(string name, shared_ptr <Value> val);
    shared_ptr <Env> resolve(string name);
    shared_ptr <Value> lookUpVar(string name);
};

shared_ptr <NumberVal> Make_Number(long double val);
shared_ptr <BoolValue> Make_Bool(bool b);
shared_ptr <NullVal> Make_Null();

void InitNatives(shared_ptr<Env> env);

#endif