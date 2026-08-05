#ifndef ENV
#define ENV

#include <lexer.hpp>
#include <parser.hpp>
#include <valtypes.hpp>
#include <unordered_map>
#include <unordered_set>
#include <operations.hpp>
#include <memory>

class Env{
    private:
    std::vector <std::unique_ptr<Env>> children;
    
    public:
    std::unordered_map <std::string, std::shared_ptr<Value>> variables;
    std::unordered_set <std::string> constants;
    Env* parent = nullptr;
    int loop_depth = 0;
    std::shared_ptr<Value> declareVar(std::string name, std::shared_ptr<Value> val, bool isConst);
    std::shared_ptr<Value> assignVar(std::string name, std::shared_ptr<Value> val);
    Env* resolve(std::string name);
    std::shared_ptr<Value> lookUpVar(std::string name);
};

std::shared_ptr<NumberVal> Make_Number(long double val);
std::shared_ptr<StringVal> Make_String(std::string val);
std::shared_ptr<BoolValue> Make_Bool(bool b);
std::shared_ptr<NullVal> Make_Null();
std::shared_ptr<NativeFuncVal> Make_NFunc(FunctionCall call);

void InitNatives(Env* env);

#endif