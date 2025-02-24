#ifndef AST
#define AST

#include <vector>

enum class NodeType{
    NONE,
    PROGRAM,
    NUMERIC_L,
    IDENTIFIER,
    BINARYEXPR
};

class Stmt{
    NodeType kind = NodeType::NONE;
};

class Program : public Stmt{
    public:
    NodeType kind = NodeType::PROGRAM;
    std::vector <Stmt> body;
};

class Expr : public Stmt{

};

class BinaryExpr : public Expr{
    NodeType kind = NodeType::BINARYEXPR;
    Expr left;
    Expr right;
    std::string op = "";
};

class Identifier : public Expr{
    NodeType kind = NodeType::IDENTIFIER;
    std::string symbol = "";

    public:
    Identifier(std::string val){
        NodeType kind = NodeType::IDENTIFIER;
        this->symbol = val;
    }
};

class NumericLiteral : public Expr{
    NodeType kind = NodeType::NUMERIC_L;
    long double val;
    public:
    NumericLiteral(std::string val){
        this->val = std::stold(val);
    }
};

#endif