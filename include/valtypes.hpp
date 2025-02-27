#ifndef VALUE_TYPES
#define VALUE_TYPES

#include <string>
#include <unordered_map>
#include <vector>

enum class ValueType{
    None,
    Null,
    Number,
    Bool,
    NFUNC,
    Object
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

#endif