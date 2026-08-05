#ifndef VALUE_TYPES
#define VALUE_TYPES

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

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
    LFUNC,
    Object,
    List,
    Break,
    Continue
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

class ContinueVal : public Value{
    private:
    ValueType type = ValueType::Continue;
    public:

    ValueType getType() const override{
        return this->type;
    } 
};

class BreakVal : public Value{
    private:
    ValueType type = ValueType::Break;
    public:

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

    std::unordered_map <std::string, std::shared_ptr<Value>> properties;
    // Keeps the declaration environment alive so functions stored in this
    // object (e.g. from run()) have a valid decEnv when called later.
    std::shared_ptr<Env> owning_env;

    ValueType getType() const override{
        return this->type;
    }

    std::shared_ptr<ObjectValue> clone(){
        auto temp = std::make_shared<ObjectValue>();
        for(auto& i : this->properties) temp->properties[i.first] = i.second;
        temp->owning_env = this->owning_env;
        return temp;
    }
};

class ListValue : public Value{
    private:
    
    ValueType type = ValueType::List;
    public:

    std::vector <std::shared_ptr<Value>> v;
    ValueType consist_of = ValueType::None;
    int mapTypeCounter[10];
    int distinc_types = 0;

    std::shared_ptr<ListValue> clone(){
        auto temp = std::make_shared<ListValue>();
        temp->consist_of = this->consist_of;
        temp->distinc_types = this->distinc_types;
        temp->v.resize(this->v.size());
        for(int i = 0; i < 11; ++i) temp->mapTypeCounter[i] = this->mapTypeCounter[i];
        for(int i = 0; i < this->v.size(); ++i) temp->v[i] = this->v[i];
        return temp;
    }

    ValueType getType() const override{
        return this->type;
    }
};

struct FunctionCall{
    std::vector <std::shared_ptr<Value>> args;
    Env* env;
    std::shared_ptr<Value> (*funAddr)(std::vector<std::shared_ptr<Value>>, Env*);
};


class NativeFuncVal : public Value{
    private:

    ValueType type = ValueType::NFUNC;
    public:

    FunctionCall call;
    bool list = false;

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
    // Owns the function body AST nodes. This way a function can outlive
    // the Program that was parsed (e.g. functions returned via run()).
    std::vector <std::unique_ptr<Stmt>> body;

    ValueType getType() const override{
        return this->type;
    } 
};

#endif