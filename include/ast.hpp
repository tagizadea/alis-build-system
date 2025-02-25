#ifndef AST
#define AST

#include <vector>
#include <string>

#include <iostream> // temp

enum class NodeType{
    NONE,
    PROGRAM,
    NUMERIC_L,
    NULL_L,
    IDENTIFIER,
    BINARYEXPR
};

class Stmt{
    NodeType kind = NodeType::NONE;
    public:
    virtual NodeType getKind() const{
        return kind;
    }
    virtual ~Stmt() = default;
};

class Program : public Stmt{
    NodeType kind = NodeType::PROGRAM;
    public:
    std::vector <Stmt*> body;
    NodeType getKind() const override{
        return kind;
    }
    Program();
};

class Expr : public Stmt{
    public:
};

class BinaryExpr : public Expr{
    NodeType kind = NodeType::BINARYEXPR;
    public:
    Expr* left;
    Expr* right;
    std::string op = "";

    NodeType getKind() const override{
        return kind;
    }
};

class Identifier : public Expr{
    NodeType kind = NodeType::IDENTIFIER;
    public:
    std::string symbol = "";

    Identifier(std::string val){
        NodeType kind = NodeType::IDENTIFIER;
        this->symbol = val;
    }
    NodeType getKind() const override{
        return kind;
    }
};

class NumericLiteral : public Expr{
    NodeType kind = NodeType::NUMERIC_L;
    public:
    long double val;
    NumericLiteral(std::string val);

    NodeType getKind() const override{
        return kind;
    }
};

class NullLiteral : public Expr{
    NodeType kind = NodeType::NULL_L;
    public:
    std::string val = "Null";

    NodeType getKind() const override{
        return kind;
    }
};

#endif