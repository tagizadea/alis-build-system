#ifndef VALUE_TYPES
#define VALUE_TYPES

#include <string>
#include <unordered_map>
#include <vector>

class Stmt;
class Env;

enum class ValueType{
    None,
    Null,
    Number,
    Bool,
    String,
    NFUNC,
    FUNC,
    Object,
    List
};

class Value{
    private:
    
    ValueType type = ValueType::None;
    public:
    
    virtual ValueType getType() const{
        return this->type;
    }

    virtual ~Value() = default;
};

class NullVal : public Value{
    private:
    
    ValueType type = ValueType::Null;
    public:
    std::string val = "Null";

    ValueType getType() const override{
        return this->type;
    } 
};

class NumberVal : public Value{
    private:
    
    ValueType type = ValueType::Number;
    public:
    long double val = 0.0;

    ValueType getType() const override{
        return this->type;
    } 
};

class StringVal : public Value{
    private:
    
    ValueType type = ValueType::String;
    public:
    std::string val = "";

    ValueType getType() const override{
        return this->type;
    } 
};

class BoolValue : public Value{
    private:
    
    ValueType type = ValueType::Bool;
    public:
    bool val;

    ValueType getType() const override{
        return this->type;
    } 
};

class ObjectValue : public Value{
    private:
    
    ValueType type = ValueType::Object;
    public:

    std::unordered_map <std::string, Value*> properties;

    ValueType getType() const override{
        return this->type;
    } 
};

class ListValue : public Value{
    private:
    
    ValueType type = ValueType::List;
    public:

    std::vector <Value*> v;

    ValueType getType() const override{
        return this->type;
    } 
};

struct FunctionCall{
    std::vector <Value*> args;
    Env* env;
    Value* (*funAddr)(std::vector<Value*>, Env*);
};


class NativeFuncVal : public Value{
    private:

    ValueType type = ValueType::NFUNC;
    public:

    FunctionCall call;

    ValueType getType() const override{
        return this->type;
    } 
};

class FunctionVal : public Value{
    private:

    ValueType type = ValueType::FUNC;
    public:

    std::string name;
    std::vector <std::string> params;
    Env* decEnv;
    std::vector <Stmt*> body;

    ValueType getType() const override{
        return this->type;
    } 
};

#endif